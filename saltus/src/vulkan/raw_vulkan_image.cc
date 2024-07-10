#include "saltus/vulkan/raw_vulkan_image.hh"
#include <cstring>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/raw_command_buffer.hh"
#include "saltus/vulkan/raw_vulkan_buffer.hh"
#include "saltus/vulkan/raw_vulkan_fence.hh"

namespace saltus::vulkan
{
    RawVulkanImage::Builder::Builder(std::shared_ptr<VulkanDevice> device_):
        device(device_)
    { }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_size(uint32_t width, uint32_t height, uint32_t depth)
    {
        with_size({{ width, height, depth }});
        return *this;
    }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_size(matrix::Vector2<uint32_t> size_)
    {
        with_size({{ size_.x(), size_.y(), 1 }});
        return *this;
    }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_size(matrix::Vector3<uint32_t> size_)
    {
        size = size_;
        return *this;
    }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_usage(VkImageUsageFlags usage_)
    {
        usage |= usage_;
        return *this;
    }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_format(VkFormat format_)
    {
        format = format_;
        return *this;
    }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_array_layers(uint32_t nls)
    {
        array_layers = nls;
        return *this;
    }

    RawVulkanImage::Builder &RawVulkanImage::Builder::with_image_type(VkImageType newtype)
    {
        image_type = newtype;
        return *this;
    }

    std::unique_ptr<RawVulkanImage> RawVulkanImage::Builder::build()
    {
        return std::make_unique<RawVulkanImage>(*this);
    }

    using BB = RawVulkanImage::BarrierBuilder;

    BB::BarrierBuilder(RawVulkanImage &img): BarrierBuilder(img.handle())
    {
        
    }
    BB::BarrierBuilder(VkImage img)
    {
        image_mem_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_mem_barrier.image = img;
        image_mem_barrier.subresourceRange.layerCount = 1;
        image_mem_barrier.subresourceRange.levelCount = 1;
    }

    BB &BB::with_old_layout(VkImageLayout val)
    {
        image_mem_barrier.oldLayout = val;
        return *this;
    }
    BB &BB::with_new_layout(VkImageLayout val)
    {
        image_mem_barrier.newLayout = val;
        return *this;
    }

    BB &BB::with_src_access_mask(VkAccessFlags val)
    {
        image_mem_barrier.srcAccessMask = val;
        return *this;
    }
    BB &BB::with_dst_access_mask(VkAccessFlags val)
    {
        image_mem_barrier.dstAccessMask = val;
        return *this;
    }

    BB &BB::with_aspect_mask(VkImageAspectFlags val)
    {
        image_mem_barrier.subresourceRange.aspectMask = val;
        return *this;
    }
    BB &BB::with_base_mip_level(uint32_t val)
    {
        image_mem_barrier.subresourceRange.baseMipLevel = val;
        return *this;
    }
    BB &BB::with_level_count(uint32_t val)
    {
        image_mem_barrier.subresourceRange.levelCount = val;
        return *this;
    }
    BB &BB::with_base_array_layer(uint32_t val)
    {
        image_mem_barrier.subresourceRange.baseArrayLayer = val;
        return *this;
    }
    BB &BB::with_layer_count(uint32_t val)
    {
        image_mem_barrier.subresourceRange.levelCount = val;
        return *this;
    }

    BB &BB::with_src_stage_mask(VkPipelineStageFlags val)
    {
        src_stage_mask = val;
        return *this;
    }
    BB &BB::with_dst_stage_mask(VkPipelineStageFlags val)
    {
        dst_stage_mask = val;
        return *this;
    }

    void BB::build(VkCommandBuffer buffer)
    {
        vkCmdPipelineBarrier(
            buffer,
            src_stage_mask, dst_stage_mask,
            0,
            0, nullptr,
            0, nullptr,
            1, &image_mem_barrier
        );
    }

    RawVulkanImage::RawVulkanImage(const Builder &builder):
        device_(builder.device),
        size_(builder.size),
        array_layers_(builder.array_layers),
        image_type_(builder.image_type),
        format_(builder.format)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = builder.image_type;
        imageInfo.extent.width = size_.x();
        imageInfo.extent.height = size_.y();
        imageInfo.extent.depth = size_.z();
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = builder.array_layers;
        imageInfo.format = builder.format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = builder.usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VkResult result = vkCreateImage(*device_, &imageInfo, nullptr, &image_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create image");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(*device_, image_, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = device_->find_mem_type(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        result = vkAllocateMemory(*device_, &allocInfo, nullptr, &image_memory_);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Could not allocate image memory");
        }

        result = vkBindImageMemory(*device_, image_, image_memory_, 0);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Could not bind memory");
        }
    }

    RawVulkanImage::~RawVulkanImage()
    {
        if (image_memory_ != VK_NULL_HANDLE)
            vkFreeMemory(*device_, image_memory_, nullptr);
        if (image_ != VK_NULL_HANDLE)
            vkDestroyImage(*device_, image_, nullptr);
    }

    const std::shared_ptr<VulkanDevice> &RawVulkanImage::device() const
    {
        return device_;
    }

    const matrix::Vector3<uint32_t> &RawVulkanImage::size() const
    {
        return size_;
    }

    const uint32_t &RawVulkanImage::array_layers() const
    {
        return array_layers_;
    }

    const VkImageType &RawVulkanImage::image_type() const
    {
        return image_type_;
    }

    const VkImage &RawVulkanImage::handle() const
    {
        return image_;
    }

    const VkDeviceMemory &RawVulkanImage::image_memory() const
    {
        return image_memory_;
    }

    const VkFormat &RawVulkanImage::format() const
    {
        return format_;
    }

    void RawVulkanImage::write(uint8_t *data, size_t size)
    {
        RawVulkanBuffer buffer(device_, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        buffer.alloc(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void *buf = buffer.map(0, VK_WHOLE_SIZE);
        memcpy(buf, data, size);
        buffer.unmap();
        write(buffer);
    }

    void RawVulkanImage::write(const RawVulkanBuffer &buffer)
    {
        RawCommandBuffer rcb(device_);
        
        rcb.begin();

        RawVulkanImage::BarrierBuilder(*this)
            .with_old_layout(VK_IMAGE_LAYOUT_UNDEFINED)
            .with_new_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            .with_src_access_mask(0)
            .with_dst_access_mask(VK_ACCESS_TRANSFER_WRITE_BIT)
            .with_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT)
            .with_src_stage_mask(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
            .with_dst_stage_mask(VK_PIPELINE_STAGE_TRANSFER_BIT)
            .build(rcb.handle());

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = { size_.x(), size_.y(), size_.z() };

        vkCmdCopyBufferToImage(
            rcb.handle(),
            buffer,
            image_,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        RawVulkanImage::BarrierBuilder{*this}
            .with_old_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            .with_new_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            .with_src_access_mask(VK_ACCESS_TRANSFER_WRITE_BIT)
            .with_dst_access_mask(VK_ACCESS_SHADER_READ_BIT)
            .with_aspect_mask(VK_IMAGE_ASPECT_COLOR_BIT)
            .with_src_stage_mask(VK_PIPELINE_STAGE_TRANSFER_BIT)
            .with_dst_stage_mask(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
            .build(rcb.handle());

        RawVulkanFence fence(device_);
        rcb.end_and_submit(device_->graphics_queue(), &fence);
        fence.wait();
    }
} // namespace saltus::vulkan
