#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/raw_vulkan_buffer.hh"
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
            uint32_t mip_levels = 1;
            VkImageUsageFlags usage = 0;
            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
            uint32_t array_layers = 1;
            VkImageType image_type = VK_IMAGE_TYPE_2D;
            VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;

            Builder(std::shared_ptr<VulkanDevice> device);
            Builder(const Builder&) = delete;
            Builder(Builder&&) = delete;
            Builder& operator=(const Builder&) = delete;
            Builder& operator=(Builder&&) = delete;

            Builder &with_size(uint32_t with, uint32_t height, uint32_t depth = 1);
            Builder &with_size(matrix::Vector2<uint32_t> size);
            Builder &with_size(matrix::Vector3<uint32_t> size);
            Builder &with_mip_levels(uint32_t mip_levels);
            Builder &with_usage(VkImageUsageFlags usage);
            Builder &with_format(VkFormat format);
            Builder &with_array_layers(uint32_t layout_count);
            Builder &with_image_type(VkImageType newtype);
            Builder &with_sample_count(VkSampleCountFlagBits newcount);

            std::unique_ptr<RawVulkanImage> build();
        };

        struct BarrierBuilder
        {
            VkImageMemoryBarrier image_mem_barrier{};

            VkPipelineStageFlags src_stage_mask;
            VkPipelineStageFlags dst_stage_mask;

            BarrierBuilder(RawVulkanImage &);
            BarrierBuilder(VkImage);

            BarrierBuilder &with_old_layout(VkImageLayout);
            BarrierBuilder &with_new_layout(VkImageLayout);

            BarrierBuilder &with_src_access_mask(VkAccessFlags);
            BarrierBuilder &with_dst_access_mask(VkAccessFlags);

            BarrierBuilder &with_aspect_mask(VkImageAspectFlags);
            BarrierBuilder &with_base_mip_level(uint32_t);
            BarrierBuilder &with_level_count(uint32_t);
            BarrierBuilder &with_base_array_layer(uint32_t);
            BarrierBuilder &with_layer_count(uint32_t);

            BarrierBuilder &with_src_stage_mask(VkPipelineStageFlags);
            BarrierBuilder &with_dst_stage_mask(VkPipelineStageFlags);

            void build(VkCommandBuffer buffer);
        };

        RawVulkanImage(const Builder &);
        ~RawVulkanImage();

        RawVulkanImage(const RawVulkanImage&) = delete;
        RawVulkanImage(RawVulkanImage&&) = delete;
        RawVulkanImage& operator=(const RawVulkanImage&) = delete;
        RawVulkanImage& operator=(RawVulkanImage&&) = delete;

        const std::shared_ptr<VulkanDevice> &device() const;
        const matrix::Vector3<uint32_t> &size() const;

        const uint32_t &mip_levels() const;
        const uint32_t &array_layers() const;
        const VkImageType &image_type() const;

        const VkImage &handle() const;
        const VkDeviceMemory &image_memory() const;
        const VkFormat &format() const;

        void write(uint8_t *data, size_t size, uint32_t array_index = 0);
        void write(const RawVulkanBuffer &buffer, uint32_t array_index = 0);

    private:
        std::shared_ptr<VulkanDevice> device_;
        matrix::Vector3<uint32_t> size_;

        uint32_t mip_levels_;
        uint32_t array_layers_;
        VkImageType image_type_;
        VkFormat format_;

        VkImage image_ = VK_NULL_HANDLE;
        VkDeviceMemory image_memory_ = VK_NULL_HANDLE;
    };
} // namespace saltus::vulkan
