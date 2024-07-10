#include "saltus/vulkan/vulkan_image.hh"
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/raw_vulkan_image.hh"

namespace saltus::vulkan
{

    VkFormat image_format_to_vk_image_format(ImageFormat format)
    {
        switch (format.pixel_format)
        {
        case ImagePixelFormat::R:
            switch (format.data_type)
            {
            case ImageDataType::u8:
                return VK_FORMAT_R8_UINT;
            case ImageDataType::u8_norm:
                return VK_FORMAT_R8_UNORM;
            case ImageDataType::i8:
                return VK_FORMAT_R8_SINT;
            case ImageDataType::i8_norm:
                return VK_FORMAT_R8_SNORM;
            case ImageDataType::srgb8:
                return VK_FORMAT_R8_SRGB;
            case ImageDataType::u16:
                return VK_FORMAT_R16_UINT;
            case ImageDataType::u16_norm:
                return VK_FORMAT_R16_UNORM;
            case ImageDataType::i16:
                return VK_FORMAT_R16_SINT;
            case ImageDataType::i16_norm:
                return VK_FORMAT_R16_SNORM;
            case ImageDataType::u32:
                return VK_FORMAT_R32_UINT;
            case ImageDataType::i32:
                return VK_FORMAT_R32_SINT;
            case ImageDataType::u64:
                return VK_FORMAT_R64_UINT;
            case ImageDataType::i64:
                return VK_FORMAT_R64_SINT;
            case ImageDataType::f32:
                return VK_FORMAT_R32_SFLOAT;
            case ImageDataType::f64:
                return VK_FORMAT_R64_SFLOAT;
            }
            break;
        case ImagePixelFormat::RG:
            switch (format.data_type)
            {
            case ImageDataType::u8:
                return VK_FORMAT_R8G8_UINT;
            case ImageDataType::u8_norm:
                return VK_FORMAT_R8G8_UNORM;
            case ImageDataType::i8:
                return VK_FORMAT_R8G8_SINT;
            case ImageDataType::i8_norm:
                return VK_FORMAT_R8G8_SNORM;
            case ImageDataType::srgb8:
                return VK_FORMAT_R8G8_SRGB;
            case ImageDataType::u16:
                return VK_FORMAT_R16G16_UINT;
            case ImageDataType::u16_norm:
                return VK_FORMAT_R16G16_UNORM;
            case ImageDataType::i16:
                return VK_FORMAT_R16G16_SINT;
            case ImageDataType::i16_norm:
                return VK_FORMAT_R16G16_SNORM;
            case ImageDataType::u32:
                return VK_FORMAT_R32G32_UINT;
            case ImageDataType::i32:
                return VK_FORMAT_R32G32_SINT;
            case ImageDataType::u64:
                return VK_FORMAT_R64G64_UINT;
            case ImageDataType::i64:
                return VK_FORMAT_R64G64_SINT;
            case ImageDataType::f32:
                return VK_FORMAT_R32G32_SFLOAT;
            case ImageDataType::f64:
                return VK_FORMAT_R64G64_SFLOAT;
            }
            break;
        case ImagePixelFormat::RGB:
            switch (format.data_type)
            {
            case ImageDataType::u8:
                return VK_FORMAT_R8G8B8_UINT;
            case ImageDataType::u8_norm:
                return VK_FORMAT_R8G8B8_UNORM;
            case ImageDataType::i8:
                return VK_FORMAT_R8G8B8_SINT;
            case ImageDataType::i8_norm:
                return VK_FORMAT_R8G8B8_SNORM;
            case ImageDataType::srgb8:
                return VK_FORMAT_R8G8B8_SRGB;
            case ImageDataType::u16:
                return VK_FORMAT_R16G16B16_UINT;
            case ImageDataType::u16_norm:
                return VK_FORMAT_R16G16B16_UNORM;
            case ImageDataType::i16:
                return VK_FORMAT_R16G16B16_SINT;
            case ImageDataType::i16_norm:
                return VK_FORMAT_R16G16B16_SNORM;
            case ImageDataType::u32:
                return VK_FORMAT_R32G32B32_UINT;
            case ImageDataType::i32:
                return VK_FORMAT_R32G32B32_SINT;
            case ImageDataType::u64:
                return VK_FORMAT_R64G64B64_UINT;
            case ImageDataType::i64:
                return VK_FORMAT_R64G64B64_SINT;
            case ImageDataType::f32:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case ImageDataType::f64:
                return VK_FORMAT_R64G64B64_SFLOAT;
            }
            break;
        case ImagePixelFormat::RGBA:
            switch (format.data_type)
            {
            case ImageDataType::u8:
                return VK_FORMAT_R8G8B8A8_UINT;
            case ImageDataType::u8_norm:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case ImageDataType::i8:
                return VK_FORMAT_R8G8B8A8_SINT;
            case ImageDataType::i8_norm:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case ImageDataType::srgb8:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case ImageDataType::u16:
                return VK_FORMAT_R16G16B16A16_UINT;
            case ImageDataType::u16_norm:
                return VK_FORMAT_R16G16B16A16_UNORM;
            case ImageDataType::i16:
                return VK_FORMAT_R16G16B16A16_SINT;
            case ImageDataType::i16_norm:
                return VK_FORMAT_R16G16B16A16_SNORM;
            case ImageDataType::u32:
                return VK_FORMAT_R32G32B32A32_UINT;
            case ImageDataType::i32:
                return VK_FORMAT_R32G32B32A32_SINT;
            case ImageDataType::u64:
                return VK_FORMAT_R64G64B64A64_UINT;
            case ImageDataType::i64:
                return VK_FORMAT_R64G64B64A64_SINT;
            case ImageDataType::f32:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case ImageDataType::f64:
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            }
            break;
        case ImagePixelFormat::BGR:
            switch (format.data_type)
            {
            case ImageDataType::u8:
                return VK_FORMAT_B8G8R8_UINT;
            case ImageDataType::u8_norm:
                return VK_FORMAT_B8G8R8_UNORM;
            case ImageDataType::i8:
                return VK_FORMAT_B8G8R8_SINT;
            case ImageDataType::i8_norm:
                return VK_FORMAT_B8G8R8_SNORM;
            case ImageDataType::srgb8:
                return VK_FORMAT_B8G8R8_SRGB;
            default:
                throw std::runtime_error("BGR Format only support 8bit subpixels");
            }
            break;
        case ImagePixelFormat::BGRA:
            switch (format.data_type)
            {
            case ImageDataType::u8:
                return VK_FORMAT_B8G8R8A8_UINT;
            case ImageDataType::u8_norm:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case ImageDataType::i8:
                return VK_FORMAT_B8G8R8A8_SINT;
            case ImageDataType::i8_norm:
                return VK_FORMAT_B8G8R8A8_SNORM;
            case ImageDataType::srgb8:
                return VK_FORMAT_B8G8R8A8_SRGB;
            default:
                throw std::runtime_error("BGRA Format only support 8bit subpixels");
            }
            break;
        }
        throw std::runtime_error("unreachable");
    }

    VulkanImage::VulkanImage(
        std::shared_ptr<VulkanDevice> device,
        ImageCreateInfo info
    ): Image(info), device_(device) {
        VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (info.usages.sampled)
            usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (info.usages.storage)
            usage |= VK_IMAGE_USAGE_STORAGE_BIT;

        image_ = std::make_unique<RawVulkanImage>(RawVulkanImage::Builder{device}
            .with_size(info.width, info.height)
            .with_usage(usage)
            .with_format(image_format_to_vk_image_format(info.format))
        );

        image_->write(info.initial_data, info.format.bytes_per_pixel() * info.width * info.height);
    }

    VulkanImage::~VulkanImage() = default;

    const std::shared_ptr<VulkanDevice> &VulkanImage::device() const
    {
        return device_;
    }

    const std::shared_ptr<RawVulkanImage> &VulkanImage::raw_image() const
    {
        return image_;
    }

    void VulkanImage::write(uint8_t *data)
    {
        image_->write(data, format().bytes_per_pixel() * width() * height());
    }
} // namespace saltus::vulkan
