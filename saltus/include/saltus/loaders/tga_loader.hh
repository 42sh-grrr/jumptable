#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace saltus::loaders::tga
{
    struct TgaImage {
        uint32_t width;
        uint32_t height;
        std::vector<uint8_t> data;
        uint32_t bytesPerPixel;
    };

    TgaImage load_tga_image(const std::string& path);
} // namespace saltus::loaders::tga
