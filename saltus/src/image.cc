#include "saltus/image.hh"
#include <stdexcept>

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
        std::size_t size;
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
    
    Image::Image(ImageCreateInfo info):
        width_(info.width), height_(info.height), usages_(info.usages),
        format_(info.format)
    {
        if (!info.usages.sampled && !info.usages.storage)
            throw std::runtime_error("Images must have at least one usage");
    }

    Image::~Image()
    { }

    const uint32_t &Image::width() const
    {
        return width_;
    }

    const uint32_t &Image::height() const
    {
        return height_;
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
