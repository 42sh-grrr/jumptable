#include "saltus/image.hh"
#include <cmath>
#include <stdexcept>
#include "logger/level.hh"

namespace saltus
{
    ImageUsages &ImageUsages::with_sampled()
    {
        sampled = true;
        return *this;
    }

    ImageUsages &ImageUsages::with_storage()
    {
        storage = true;
        return *this;
    }

    std::size_t ImageFormat::bytes_per_pixel() const
    {
        std::size_t size = 1;
        switch (pixel_format)
        {
        case ImagePixelFormat::R:
            size = 1;
            break;
        case ImagePixelFormat::RG:
            size = 2;
            break;
        case ImagePixelFormat::BGR:
        case ImagePixelFormat::RGB:
            size = 3;
            break;
        case ImagePixelFormat::BGRA:
        case ImagePixelFormat::RGBA:
            size = 4;
            break;
        }
        switch (data_type)
        {
        case ImageDataType::u8:
        case ImageDataType::u8_norm:
        case ImageDataType::i8:
        case ImageDataType::i8_norm:
        case ImageDataType::srgb8:
            size *= 1;
            break;
        case ImageDataType::u16:
        case ImageDataType::u16_norm:
        case ImageDataType::i16:
        case ImageDataType::i16_norm:
            size *= 2;
            break;
        case ImageDataType::u32:
        case ImageDataType::i32:
        case ImageDataType::f32:
        case ImageDataType::f64:
            size *= 4;
            break;
        case ImageDataType::u64:
        case ImageDataType::i64:
            size *= 8;
            break;
        }
        return size;
    }

    uint32_t max_image_mip_levels(matrix::Vector3<uint32_t> extent)
    {
        float max_mip_levels = std::floor(std::log2(std::max(
            static_cast<float>(extent.x()),std::max(
            static_cast<float>(extent.y()),
            static_cast<float>(extent.z())
        ))));
        return static_cast<uint32_t>(max_mip_levels)+1;
    }
    
    Image::Image(ImageCreateInfo &info):
        dimensions_(info.dimensions),
        mip_levels_(info.mip_levels),
        usages_(info.usages),
        format_(info.format)
    {
        if (!info.usages.sampled && !info.usages.storage)
            throw std::runtime_error("Images must have at least one usage");

        auto max_mip_levels = max_image_mip_levels(info.dimensions);
        if (info.mip_levels > max_mip_levels)
        {
            logger::warn() << "Specified mip levels is above maximum (" << info.mip_levels << " > " << max_mip_levels << "), it has been clamped\n";
            mip_levels_ = max_mip_levels;
            info.mip_levels = max_mip_levels;
        }
    }

    Image::~Image()
    { }

    const matrix::Vector3<uint32_t> &Image::dimensions() const
    {
        return dimensions_;
    }

    const ImageUsages &Image::usages() const
    {
        return usages_;
    }

    const ImageFormat &Image::format() const
    {
        return format_;
    }
} // namespace saltus
