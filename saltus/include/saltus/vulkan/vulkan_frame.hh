#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/renderer.hh"
#include "saltus/vulkan/vulkan_render_target.hh"

namespace saltus::vulkan
{
    class VulkanFrame
    {
    public:
        VulkanFrame(std::shared_ptr<VulkanRenderTarget> render_target, uint32_t frame_index);
        VulkanFrame(VulkanFrame&&);
        ~VulkanFrame();

        VulkanFrame(const VulkanFrame&) = delete;
        VulkanFrame& operator=(const VulkanFrame&) = delete;
        VulkanFrame& operator=(VulkanFrame&&) = delete;

        const VkCommandBuffer &command_buffer() const;
        const VkSemaphore &image_available_semaphore() const;
        const VkSemaphore &render_finished_semaphore() const;
        const VkFence &in_flight_fence() const;

        void record(const RenderInfo &info, uint32_t image_index);

    private:
        std::shared_ptr<VulkanRenderTarget> render_target_ = nullptr;
        uint32_t frame_index_;

        VkCommandBuffer command_buffer_ = VK_NULL_HANDLE;
        VkSemaphore image_available_semaphore_ = VK_NULL_HANDLE;
        VkSemaphore render_finished_semaphore_ = VK_NULL_HANDLE;
        VkFence in_flight_fence_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan

