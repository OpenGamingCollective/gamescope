#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

namespace gamescope
{
    // A commit's first measured output presentation. Repeated scanout must not
    // replace its timestamp. This object owns no Wayland or compositor objects.
    struct PresentationTiming
    {
        PresentationTiming( uint32_t id, uint64_t desired )
            : presentID{ id }, desiredPresentTime{ desired } {}

        void Presented( uint64_t time )
        {
            uint64_t expected = 0;
            actualPresentTime.compare_exchange_strong( expected, time );
        }

        const uint32_t presentID;
        const uint64_t desiredPresentTime;
        std::atomic<uint64_t> actualPresentTime{ 0 };
    };

    using PresentationTimings = std::vector<std::shared_ptr<PresentationTiming>>;
}
