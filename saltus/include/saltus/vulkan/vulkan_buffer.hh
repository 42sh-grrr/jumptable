#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/buffer.hh"
#include "saltus/vulkan/raw_vulkan_buffer.hh"
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

        RawVulkanBuffer &raw_buffer();
        const RawVulkanBuffer &raw_buffer() const;

        void write(
            const uint8_t *data,
            uint64_t offset = 0,
            std::optional<uint64_t> size = std::nullopt
        ) override;

    private:
        std::shared_ptr<VulkanDevice> device_;

        std::unique_ptr<RawVulkanBuffer> raw_buffer_;
    };
}
