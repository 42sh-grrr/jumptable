#include "saltus/vulkan/vulkan_buffer.hh"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanBuffer::VulkanBuffer(
        std::shared_ptr<VulkanDevice> device,
        BufferCreateInfo info
    ): Buffer(info), device_(device)
    {
        VkBufferUsageFlags usages = 0;

        if (info.usages.uniform)
            usages |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (info.usages.index)
            usages |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (info.usages.vertex)
            usages |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        if (usages == 0)
            throw std::runtime_error("Cannot create a vulkan buffer with 0 usages");

        raw_buffer_ = std::make_unique<RawVulkanBuffer>(
            device, info.size, usages
        );

        raw_buffer_->alloc(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (info.data)
            write(info.data);
    }

    VulkanBuffer::~VulkanBuffer()
    { }

    RawVulkanBuffer &VulkanBuffer::raw_buffer()
    {
        return *raw_buffer_;
    }

    const RawVulkanBuffer &VulkanBuffer::raw_buffer() const
    {
        return *raw_buffer_;
    }

    void VulkanBuffer::write(
        const uint8_t *data,
        uint64_t offset,
        std::optional<uint64_t> size
    ) {
        void *bdata = raw_buffer_->map(0, VK_WHOLE_SIZE);
        memcpy(bdata, data, this->size());
        raw_buffer_->unmap();
    }
}
