#pragma once

#include <stdexcept>
#include "saltus/bind_group_layout.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    inline VkDescriptorType binding_type_to_descriptor_type(BindingType type)
    {
        switch (type)
        {
        case BindingType::UniformBuffer:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case BindingType::StorageBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        default:
            throw std::runtime_error("Unknown binding type");
        }
    }

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
