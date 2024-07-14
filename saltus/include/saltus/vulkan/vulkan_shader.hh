#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "saltus/shader.hh"
#include "saltus/vulkan/fwd.hh"

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

        VkPipelineShaderStageCreateInfo stage_create_info() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkShaderModule module_;
    };
} // namespace saltus
