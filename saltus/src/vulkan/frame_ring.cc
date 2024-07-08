#include "saltus/vulkan/frame_ring.hh"

#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/config.hh"

namespace saltus::vulkan
{
    FrameRing::FrameRing(VulkanRenderer &renderer):
        renderer_(renderer)
    { }

    FrameRing::~FrameRing() = default;

    uint32_t FrameRing::frame_count() const
    {
        return frames_.size();
    }

    VulkanFrame &FrameRing::frame()
    {
        return frames_.at(current_frame_);
    }

    const VulkanFrame &FrameRing::frame() const
    {
        return frames_.at(current_frame_);
    }

    uint32_t FrameRing::current_frame() const
    {
        return current_frame_;
    }

    void FrameRing::resize(uint32_t new_count)
    {
        if (new_count > MAX_FRAMES_IN_FLIGHT)
            throw std::range_error("Cannot have more than max frames frames");

        // Free resources of deleted frames
        for (uint32_t i = frame_count(); i > new_count; i--)
        {
            for (const auto &resource : resources_)
                resource->free(i-1);
        }

        while (frames_.size() > new_count)
            frames_.pop_back();
        while (frames_.size() < new_count)
            frames_.emplace_back(renderer_.render_target(), frames_.size());
    }

    void FrameRing::next_frame()
    {
        current_frame_ = (current_frame_ + 1) % frame_count();
    }
} // namespace saltus::vulkan


