#include "saltus/vulkan/vulkan_material.hh"

#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_shader.hh"

namespace saltus::vulkan
{
    VulkanMaterial::VulkanMaterial(
        std::shared_ptr<VulkanDevice> device,
        MaterialCreateInfo create_info
    ): Material(create_info), device_(device)
    {
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

    const std::shared_ptr<VulkanDevice> &VulkanMaterial::device() const {
        return device_;
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
