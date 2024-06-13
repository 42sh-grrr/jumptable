#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/buffer.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class VulkanBuffer: public Buffer
    {
    public:
        VulkanBuffer(
            std::shared_ptr<VulkanDevice> device,
            BufferCreateInfo info
        );
        ~VulkanBuffer();

        operator VkBuffer() const;

        VkBuffer buffer() const;

        void assign(const uint8_t *data);

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkBuffer buffer_ = VK_NULL_HANDLE;
        VkDeviceMemory memory_ = VK_NULL_HANDLE;

        void alloc(VkMemoryPropertyFlags memory_properties);
        void *map();
        void unmap();

        uint32_t find_mem_type(uint32_t type_filter_, VkMemoryPropertyFlags properties);
    };
}
