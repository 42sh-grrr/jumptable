#include "saltus/vulkan/vulkan_bind_group_layout.hh"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanBindGroupLayout::VulkanBindGroupLayout(
        std::shared_ptr<VulkanDevice> device,
        BindGroupLayoutCreateInfo info
    ): BindGroupLayout(info), device_(device), layout_(VK_NULL_HANDLE)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        for (const auto &binding : info.bindings)
        {
            bindings.push_back({
                .binding = binding.binding_id,
                .descriptorType = binding_type_to_descriptor_type(binding.type),
                .descriptorCount = binding.count,
                // TODO: Add something in bindings to make it granular
                //       i have in mind a bitfield abstraction or smth
                .stageFlags = VK_SHADER_STAGE_ALL,
                .pImmutableSamplers = nullptr,
            });
        }

        VkDescriptorSetLayoutCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.bindingCount = bindings.size();
        create_info.pBindings = bindings.data();
        VkResult result = vkCreateDescriptorSetLayout(*device_, &create_info, nullptr, &layout_);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create layout");
        }
    }

    VulkanBindGroupLayout::~VulkanBindGroupLayout()
    {
        if (layout_ != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(*device_, layout_, nullptr);
    }

    const std::shared_ptr<VulkanDevice> &VulkanBindGroupLayout::device() const
    {
        return device_;
    }

    const VkDescriptorSetLayout &VulkanBindGroupLayout::layout() const
    {
        return layout_;
    }
} // namespace saltus::vulkan

