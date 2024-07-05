#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/material.hh"
#include "saltus/vulkan/vulkan_bind_group_layout.hh"
#include "saltus/vulkan/vulkan_device.hh"
#include "saltus/vulkan/vulkan_shader.hh"

namespace saltus::vulkan
{
    class VulkanMaterial: public Material
    {
    public:
        VulkanMaterial(std::shared_ptr<VulkanDevice>, MaterialCreateInfo create_info);
        ~VulkanMaterial();

        const std::shared_ptr<VulkanDevice> &device() const;
        const std::vector<std::shared_ptr<VulkanBindGroupLayout>> &bind_group_layouts() const;

        const std::shared_ptr<VulkanShader> &vertex_shader() const;
        const std::shared_ptr<VulkanShader> &fragment_shader() const;
    private:
        std::shared_ptr<VulkanDevice> device_;
        std::vector<std::shared_ptr<VulkanBindGroupLayout>> bind_group_layouts_;

        std::shared_ptr<VulkanShader> vertex_shader_;
        std::shared_ptr<VulkanShader> fragment_shader_;
    };
} // namespace saltus::vulkan


