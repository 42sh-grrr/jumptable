#include "saltus/vulkan/vulkan_sampler.hh"

#include <cstdlib>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "matrix/vector.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    VkFilter vk_filter_to_filter(SamplerFilter filter)
    {
        switch (filter)
        {
        case SamplerFilter::Nearest:
            return VK_FILTER_NEAREST;
        case SamplerFilter::Linear:
            return VK_FILTER_LINEAR;
        }
        throw std::runtime_error("Invalid filter");
    }

    VkSamplerMipmapMode vk_filter_to_mipmapmode(SamplerFilter filter)
    {
        switch (filter)
        {
        case SamplerFilter::Nearest:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case SamplerFilter::Linear:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
        throw std::runtime_error("Invalid filter");
    }

    VkSamplerAddressMode vk_wraping_to_address_mode(SamplerWraping filter)
    {
        switch (filter)
        {
        case SamplerWraping::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerWraping::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerWraping::ClampToEdge:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerWraping::ClampToBorder:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        }
        throw std::runtime_error("Invalid sampler wraping");
    }

    bool eqf(float a, float b)
    {
        return std::abs(a - b) < 0.00001;
    }
    bool eqf(matrix::Vector4F a, matrix::Vector4F b)
    {
        for (int i = 0; i < 4; i++)
            if (!eqf(a[i], b[i]))
                return false;
        return true;
    }

    VkBorderColor color_to_border_color(matrix::Vector4F color)
    {
        if (eqf(color, matrix::Vector4F{{ 0., 0., 0., 0. }}))
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        if (eqf(color, matrix::Vector4F{{ 0., 0., 0., 1. }}))
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        if (eqf(color, matrix::Vector4F{{ 1., 1., 1., 1. }}))
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        return VK_BORDER_COLOR_FLOAT_CUSTOM_EXT;
    }

    VulkanSampler::VulkanSampler(
        std::shared_ptr<VulkanDevice> device,
        SamplerCreateInfo info
    ): Sampler(info), device_(device) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = vk_filter_to_filter(info.mag_filter);
        samplerInfo.minFilter = vk_filter_to_filter(info.min_filter);
        samplerInfo.mipmapMode = vk_filter_to_mipmapmode(info.mipmap_mode);
        samplerInfo.addressModeU = vk_wraping_to_address_mode(info.wrap_u);
        samplerInfo.addressModeV = vk_wraping_to_address_mode(info.wrap_v);
        samplerInfo.addressModeW = vk_wraping_to_address_mode(info.wrap_w);
        samplerInfo.mipLodBias = info.mip_lod_bias;

        if (info.anisotropic_filtering)
        {
            samplerInfo.anisotropyEnable = VK_TRUE;
            samplerInfo.maxAnisotropy = device_->physical_device_properties().limits.maxSamplerAnisotropy;
        }
        else {
            samplerInfo.anisotropyEnable = VK_FALSE;
            samplerInfo.maxAnisotropy = 0.f;
        }

        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.minLod = info.min_lod;
        samplerInfo.maxLod = info.max_lod;
        samplerInfo.borderColor = color_to_border_color(info.border_color);
        samplerInfo.unnormalizedCoordinates = VK_FALSE;

        VkResult result = vkCreateSampler(*device_, &samplerInfo, nullptr, &sampler_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create sampler");
    }

    VulkanSampler::~VulkanSampler()
    {
        if (device_ && sampler_ != VK_NULL_HANDLE)
            vkDestroySampler(*device_, sampler_, nullptr);
    }

    const std::shared_ptr<VulkanDevice> &VulkanSampler::device() const
    {
        return device_;
    }

    const VkSampler &VulkanSampler::handle() const
    {
        return sampler_;
    }

    VulkanSampler::operator VkSampler() const
    {
        return handle();
    }
} // namespace saltus::vulkan
