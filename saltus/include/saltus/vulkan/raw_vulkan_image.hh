#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class RawVulkanImage
    {
    public:
        struct Builder
        {
            std::shared_ptr<VulkanDevice> device;

            matrix::Vector3<uint32_t> size = {{ 1, 1, 1 }};
            VkImageUsageFlags usage = 0;
            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
            uint32_t array_layers = 1;
            VkImageType image_type = VK_IMAGE_TYPE_2D;

            Builder(std::shared_ptr<VulkanDevice> device);
            Builder(const Builder&) = delete;
            Builder(Builder&&) = delete;
            Builder& operator=(const Builder&) = delete;
            Builder& operator=(Builder&&) = delete;

            Builder &with_size(uint32_t with, uint32_t height, uint32_t depth = 1);
            Builder &with_size(matrix::Vector2<uint32_t> size);
            Builder &with_size(matrix::Vector3<uint32_t> size);
            Builder &with_usage(VkImageUsageFlags usage);
            Builder &with_format(VkFormat format);
            Builder &with_array_layers(uint32_t layout_count);
            Builder &with_image_type(VkImageType newtype);

            std::unique_ptr<RawVulkanImage> build();
        };

        RawVulkanImage(const Builder &);
        ~RawVulkanImage();

        RawVulkanImage(const RawVulkanImage&) = delete;
        RawVulkanImage(RawVulkanImage&&) = delete;
        RawVulkanImage& operator=(const RawVulkanImage&) = delete;
        RawVulkanImage& operator=(RawVulkanImage&&) = delete;

        const std::shared_ptr<VulkanDevice> &device() const;
        const matrix::Vector3<uint32_t> &size() const;

        const uint32_t &array_layers() const;
        const VkImageType &image_type() const;

        const VkImage &image() const;
        const VkDeviceMemory &image_memory() const;

    private:
        std::shared_ptr<VulkanDevice> device_;
        matrix::Vector3<uint32_t> size_;

        uint32_t array_layers_;
        VkImageType image_type_;

        VkImage image_ = VK_NULL_HANDLE;
        VkDeviceMemory image_memory_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan
