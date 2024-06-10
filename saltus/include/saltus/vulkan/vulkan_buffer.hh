#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class VulkanBuffer
    {
    public:
        VulkanBuffer(
            std::shared_ptr<VulkanDevice> device,
            size_t size, VkBufferUsageFlags usage
        );
        ~VulkanBuffer();

        VulkanBuffer(const VulkanBuffer &) = delete;
        VulkanBuffer &operator =(const VulkanBuffer &) = delete;

        operator VkBuffer() const;

        VkBuffer buffer() const;
        VkDeviceMemory memory() const;

        bool is_allocated() const;
        void alloc();

        void *map();
        void unmap();

    private:
        std::shared_ptr<VulkanDevice> device_;

        size_t size_;
        VkBuffer buffer_ = VK_NULL_HANDLE;
        VkDeviceMemory memory_ = VK_NULL_HANDLE;

        uint32_t find_mem_type(uint32_t type_filter_, VkMemoryPropertyFlags properties);
    };
}
