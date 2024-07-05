#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/bind_group.hh"
#include "saltus/vulkan/vulkan_bind_group_layout.hh"
#include "saltus/vulkan/vulkan_device.hh"
namespace saltus::vulkan
{
    class VulkanBindGroup: public BindGroup
    {
    public:
        VulkanBindGroup(std::shared_ptr<VulkanDevice> device, BindGroupCreateInfo info);
        ~VulkanBindGroup();

        const std::shared_ptr<VulkanDevice> &device() const;
        const std::shared_ptr<VulkanBindGroupLayout> &bind_group_layout() const;

        const VkDescriptorSet &descriptor_set() const;

        virtual void set_binding(
            uint32_t binding_id,
            const std::shared_ptr<Buffer> &buffer,
            uint32_t array_index = 0,
            uint64_t offset = 0,
            std::optional<uint64_t> size = std::nullopt
        ) override;

    private:
        std::shared_ptr<VulkanDevice> device_;
        std::shared_ptr<VulkanBindGroupLayout> bind_group_layout_;

        VkDescriptorPool descriptor_pool_;
        VkDescriptorSet descriptor_set_;
    };
} // namespace saltus

