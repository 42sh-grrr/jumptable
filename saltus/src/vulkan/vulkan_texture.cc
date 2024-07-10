#include "saltus/vulkan/vulkan_texture.hh"

#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/raw_vulkan_image_view.hh"

namespace saltus::vulkan
{
    VulkanTexture::VulkanTexture(
        std::shared_ptr<VulkanDevice> device,
        TextureCreateInfo info
    ): Texture(info), device_(device) {
        image_ = std::dynamic_pointer_cast<VulkanImage>(info.image);
        if (!image_)
            throw std::runtime_error("Vulkan texture can only be created from vulkan images");
        sampler_ = std::dynamic_pointer_cast<VulkanSampler>(info.sampler);
        if (!sampler_)
            throw std::runtime_error("Vulkan texture can only be created from vulkan samplers");

        VkImageViewType type;
        switch (image_->raw_image()->image_type())
        {
        case VK_IMAGE_TYPE_1D:
            type = VK_IMAGE_VIEW_TYPE_1D;
            break;
        case VK_IMAGE_TYPE_2D:
            type = VK_IMAGE_VIEW_TYPE_2D;
            break;
        case VK_IMAGE_TYPE_3D:
            type = VK_IMAGE_VIEW_TYPE_3D;
            break;
        default:
            throw std::runtime_error("Invalid image type");
        }

        image_view_ = std::make_shared<RawVulkanImageView>(
            image_->raw_image(),
            image_->raw_image()->format(),
            type,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
    }

    VulkanTexture::~VulkanTexture()
    { }

    const std::shared_ptr<VulkanDevice> &VulkanTexture::device() const
    {
        return device_;
    }

    const std::shared_ptr<VulkanImage> &VulkanTexture::image() const
    {
        return image_;
    }
    const std::shared_ptr<VulkanSampler> &VulkanTexture::sampler() const
    {
        return sampler_;
    }

    const std::shared_ptr<RawVulkanImageView> &VulkanTexture::image_view() const
    {
        return image_view_;
    }
} // namespace saltus::vulkan

