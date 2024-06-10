#pragma once

#include "saltus/shader.hh"
#include "saltus/vulkan/vulkan_device.hh"

#include <memory>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    class VulkanShader: public Shader
    {
    public:
        VulkanShader(std::shared_ptr<VulkanDevice> device, ShaderCreateInfo info);
        ~VulkanShader();

        VulkanShader(const VulkanShader &) = delete;
        VulkanShader &operator =(const VulkanShader &) = delete;

        VkShaderModule module() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkShaderModule module_;
    };
} // namespace saltus
