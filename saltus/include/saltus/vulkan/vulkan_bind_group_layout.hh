#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/bind_group_layout.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    VkDescriptorType binding_type_to_descriptor_type(BindingType type);

    class VulkanBindGroupLayout: public BindGroupLayout
    {
    public:
        VulkanBindGroupLayout(std::shared_ptr<VulkanDevice>, BindGroupLayoutCreateInfo);
        ~VulkanBindGroupLayout() override;

        const std::shared_ptr<VulkanDevice> &device() const;
        const VkDescriptorSetLayout &layout() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkDescriptorSetLayout layout_;
    };
} // namespace saltus::vulkan
