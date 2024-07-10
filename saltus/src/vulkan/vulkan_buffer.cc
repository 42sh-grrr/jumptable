#include "saltus/vulkan/vulkan_buffer.hh"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/buffer.hh"
#include "saltus/vulkan/raw_vulkan_buffer.hh"

namespace saltus::vulkan
{
    VulkanBuffer::VulkanBuffer(
        std::shared_ptr<VulkanDevice> device,
        BufferCreateInfo info
    ): Buffer(info), device_(device)
    {
        VkBufferUsageFlags usages = dynamic() ? 0 : VK_BUFFER_USAGE_TRANSFER_DST_BIT;

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

        if (dynamic()) {
            raw_buffer_->alloc(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            mapped_ = raw_buffer_->map(0, VK_WHOLE_SIZE);
        }
        else
            raw_buffer_->alloc(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (info.data)
            write(info.data);
    }

    VulkanBuffer::~VulkanBuffer()
    { }

    bool VulkanBuffer::dynamic() const
    {
        return access_hint() == BufferAccessHint::Dynamic;
    }

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
        size_t actual_size = size.value_or(this->size() - offset);
        if (dynamic())
        {
            memcpy(static_cast<char*>(mapped_) + offset, data, actual_size);
        }
        else
        {

            RawVulkanBuffer staging_buffer{
                device_, actual_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            };
            staging_buffer.alloc(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            void *bdata = staging_buffer.map(0, VK_WHOLE_SIZE);
            memcpy(bdata, data, actual_size);
            staging_buffer.unmap();

            raw_buffer_->copy(staging_buffer);
        }
    }
}
