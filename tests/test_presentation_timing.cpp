#include <catch2/catch_test_macros.hpp>
#include "PresentationTiming.h"

TEST_CASE("Only a measured first presentation supplies timing", "[presentation_timing]")
{
    gamescope::PresentationTiming timing{42, 900};
    REQUIRE(timing.actualPresentTime.load() == 0);
    timing.Presented(1000);
    timing.Presented(2000); // Repeated scanout is not another application frame.
    REQUIRE(timing.presentID == 42);
    REQUIRE(timing.desiredPresentTime == 900);
    REQUIRE(timing.actualPresentTime.load() == 1000);
}

TEST_CASE("Deferred composition retains the previous frame identity", "[presentation_timing]")
{
    auto previous = std::make_shared<gamescope::PresentationTiming>(1, 0);
    auto current = std::make_shared<gamescope::PresentationTiming>(2, 0);
    gamescope::PresentationTimings output{previous};
    gamescope::PresentationTimings next{current};
    for (const auto &timing : output) timing->Presented(1000);
    REQUIRE(previous->actualPresentTime.load() == 1000);
    REQUIRE(current->actualPresentTime.load() == 0);
    output = next;
    for (const auto &timing : output) timing->Presented(2000);
    REQUIRE(current->actualPresentTime.load() == 2000);
}

TEST_CASE("A dropped commit expires without invented feedback", "[presentation_timing]")
{
    auto commit = std::make_shared<gamescope::PresentationTiming>(3, 0);
    std::weak_ptr<gamescope::PresentationTiming> pending = commit;
    REQUIRE(pending.lock()->actualPresentTime.load() == 0);
    commit.reset();
    REQUIRE(pending.expired());
}
