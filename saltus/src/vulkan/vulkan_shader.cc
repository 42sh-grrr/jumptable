#include "saltus/vulkan/vulkan_shader.hh"

#include <stdexcept>
#include <cassert>
#include <vulkan/vulkan_core.h>

#include "saltus/shader.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    VulkanShader::VulkanShader(std::shared_ptr<VulkanDevice> device, ShaderCreateInfo info)
        : Shader(info), device_(device)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = info.source_code.size();
        assert((info.source_code.size() % 4) == 0);
        create_info.pCode = reinterpret_cast<const uint32_t *>(
            info.source_code.data()
        );

        VkResult result =
            vkCreateShaderModule(device->device(), &create_info, nullptr, &module_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create shader module");
    }

    VulkanShader::~VulkanShader()
    {
        vkDestroyShaderModule(*device_, module_, nullptr);
    }

    VkShaderModule VulkanShader::module() const
    {
        return module_;
    }

    VkPipelineShaderStageCreateInfo VulkanShader::stage_create_info() const
    {
        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        switch (kind())
        {
        case ShaderKind::Fragment:
            vert_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderKind::Vertex:
            vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        }
        vert_shader_stage_info.module = module_;
        vert_shader_stage_info.pName = "main";

        return vert_shader_stage_info;
    }
} // namespace saltus::vulkan
