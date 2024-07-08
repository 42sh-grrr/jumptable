#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#include "saltus/vulkan/frame_resource.hh"
#include "saltus/vulkan/vulkan_renderer.hh"
#include "saltus/vulkan/vulkan_frame.hh"

namespace saltus::vulkan
{
    class FrameRing
    {
    public:
        FrameRing(VulkanRenderer &renderer);
        ~FrameRing();

        FrameRing(const FrameRing&) = delete;
        FrameRing(FrameRing&&) = delete;
        FrameRing& operator=(const FrameRing&) = delete;
        FrameRing& operator=(FrameRing&&) = delete;

        uint32_t frame_count() const;

        VulkanFrame &frame();
        const VulkanFrame &frame() const;

        uint32_t current_frame() const;

        void resize(uint32_t new_count);
        void next_frame();

        template <class T>
        FrameResource<T> allocate_resource(typename FrameResource<T>::Factory factory);

    private:
        VulkanRenderer &renderer_;

        uint32_t current_frame_ = 0;
        std::vector<VulkanFrame> frames_;

        std::vector<std::unique_ptr<GenericFrameResource>> resources_;
    };
} // namespace saltus::vulkan

#include <saltus/vulkan/frame_ring.hxx>
