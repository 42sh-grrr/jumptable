#include "saltus/vulkan/raw_vulkan_image.hh"
#include <vulkan/vulkan_core.h>

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

    RawVulkanImage::RawVulkanImage(const Builder &builder):
        device_(builder.device),
        size_(builder.size),
        array_layers_(builder.array_layers),
        image_type_(builder.image_type)
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

    const VkImage &RawVulkanImage::image() const
    {
        return image_;
    }

    const VkDeviceMemory &RawVulkanImage::image_memory() const
    {
        return image_memory_;
    }
} // namespace saltus::vulkan
