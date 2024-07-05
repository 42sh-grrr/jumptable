#include "saltus/vulkan/vulkan_material.hh"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_bind_group_layout.hh"
#include "saltus/vulkan/vulkan_shader.hh"

namespace saltus::vulkan
{
    VulkanMaterial::VulkanMaterial(
        std::shared_ptr<VulkanDevice> device,
        MaterialCreateInfo create_info
    ): Material(create_info), device_(device)
    {
        for (const auto &layout : create_info.bind_group_layouts)
        {
            auto vk_layout =
                std::dynamic_pointer_cast<VulkanBindGroupLayout>(layout);
            if (!layout)
                throw std::runtime_error("Vulkan material can only work with vulkan bind group");
            bind_group_layouts_.push_back(vk_layout);
        }

        auto vertex_shader =
            std::dynamic_pointer_cast<VulkanShader>(create_info.vertex_shader);
        vertex_shader_ = vertex_shader;
        auto fragment_shader =
            std::dynamic_pointer_cast<VulkanShader>(create_info.fragment_shader);
        fragment_shader_ = fragment_shader;
        if (!vertex_shader || !fragment_shader)
            throw std::runtime_error("Vulkan material can only work with vulkan shaders");
    }

    VulkanMaterial::~VulkanMaterial()
    { }

    const std::shared_ptr<VulkanDevice> &VulkanMaterial::device() const
    {
        return device_;
    }

    const std::vector<std::shared_ptr<VulkanBindGroupLayout>> &VulkanMaterial::bind_group_layouts() const
    {
        return bind_group_layouts_;
    }

    const std::shared_ptr<VulkanShader> &VulkanMaterial::vertex_shader() const
    {
        return vertex_shader_;
    }

    const std::shared_ptr<VulkanShader> &VulkanMaterial::fragment_shader() const
    {
        return fragment_shader_;
    }
} // namespace saltus::vulkan
