#include "saltus/vulkan/vulkan_shader.hh"
#include <stdexcept>
#include <cassert>

namespace saltus::vulkan
{
    VulkanShader::VulkanShader(std::shared_ptr<VulkanDevice> device, ShaderCreateInfo info)
        : device_(device)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = info.source_code_.size();
        assert((info.source_code_.size() % 4) == 0);
        create_info.pCode = reinterpret_cast<const uint32_t *>(
            info.source_code_.data()
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
} // namespace saltus::vulkan
