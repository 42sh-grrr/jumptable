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

        void set_binding(
            uint32_t binding_id,
            uint32_t array_index,
            std::span<const std::shared_ptr<Buffer>> buffers
        ) override;

    private:
        std::shared_ptr<VulkanDevice> device_;
        std::shared_ptr<VulkanBindGroupLayout> bind_group_layout_;

        VkDescriptorPool descriptor_pool_;
        VkDescriptorSet descriptor_set_;
    };
} // namespace saltus

