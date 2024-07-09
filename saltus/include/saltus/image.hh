#pragma once

#include <cstdint>
namespace saltus
{
    struct ImageUsages
    {
        bool sampled: 1;
        bool storage: 1;

        ImageUsages &with_sampled();
        ImageUsages &with_storage();
    };

    enum class ImagePixelFormat
    {
        R,
        RG,
        RGB,
        RGBA,
        BGR,
        BGRA,
    };

    enum class ImageDataType
    {
        u8,
        u8_norm,
        i8,
        i8_norm,
        srgb8,
        u16,
        u16_norm,
        i16,
        i16_norm,
        u32,
        i32,
        u64,
        i64,

        f32,
        f64,
    };

    struct ImageFormat
    {
        ImagePixelFormat pixel_format;
        ImageDataType data_type;

        std::size_t bytes_per_pixel() const;
    };

    struct ImageCreateInfo
    {
        uint32_t width;
        uint32_t height;

        ImageUsages usages;
        ImageFormat format;

        /// Can be null
        uint8_t *initial_data;
    };

    class Image
    {
    public:
        Image(ImageCreateInfo);
        virtual ~Image() = 0;

        Image(const Image&) = delete;
        Image(Image&&) = delete;
        Image& operator=(const Image&) = delete;
        Image& operator=(Image&&) = delete;

        const uint32_t &width() const;
        const uint32_t &height() const;

        const ImageUsages &usages() const;
        const ImageFormat &format() const;

        virtual void write(uint8_t *data) = 0;

    private:
        uint32_t width_;
        uint32_t height_;

        ImageUsages usages_;
        ImageFormat format_;
    };
} // namespace saltus
