#include "saltus/vulkan/raw_vulkan_image_view.hh"
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    RawVulkanImageView::RawVulkanImageView(
        std::shared_ptr<RawVulkanImage> image,
        VkFormat format,
        VkImageViewType view_type,
        VkImageAspectFlagBits aspect_mask
    ): image_(image) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image->handle();
        viewInfo.viewType = view_type;
        viewInfo.format = format;

        viewInfo.subresourceRange.aspectMask = aspect_mask;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = image->mip_levels();
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = image->array_layers();
        viewInfo.subresourceRange.layerCount = image->array_layers();

        VkResult result = vkCreateImageView(*image->device(), &viewInfo, nullptr, &view_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("failed to create texture image view!");
    }

    RawVulkanImageView::~RawVulkanImageView()
    {
        if (view_ != VK_NULL_HANDLE)
            vkDestroyImageView(*image_->device(), view_, nullptr);
    }

    const std::shared_ptr<RawVulkanImage> &RawVulkanImageView::image() const
    {
        return image_;
    }

    const VkFormat &RawVulkanImageView::format() const
    {
        return format_;
    }

    const VkImageView &RawVulkanImageView::handle() const
    {
        return view_;
    }
} // namespace saltus::vulkan
