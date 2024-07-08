#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/raw_vulkan_image.hh"

namespace saltus::vulkan
{
    class RawVulkanImageView
    {
    public:
        RawVulkanImageView(
            std::shared_ptr<RawVulkanImage> image,
            VkFormat format,
            VkImageViewType view_type,
            VkImageAspectFlagBits aspect_mask
        );
        ~RawVulkanImageView();

        RawVulkanImageView(const RawVulkanImageView&) = delete;
        RawVulkanImageView(RawVulkanImageView&&) = delete;
        RawVulkanImageView& operator=(const RawVulkanImageView&) = delete;
        RawVulkanImageView& operator=(RawVulkanImageView&&) = delete;

        const std::shared_ptr<RawVulkanImage> &image() const;
        const VkFormat &format() const;
        const VkImageView &view() const;

    private:
        std::shared_ptr<RawVulkanImage> image_;
        VkFormat format_;

        VkImageView view_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan

