
#include "saltus/loaders/tga_loader.hh"

#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

namespace saltus::loaders::tga
{
    enum class TgaColorMapType: uint8_t {
        NO_COLOR_MAP = 0,
        COLOR_MAP = 1
    };

    enum class TgaImageType: uint8_t {
        NO_IMAGE_DATA = 0,
        UNCOMPRESSED_COLOR_MAPPED = 1,
        UNCOMPRESSED_TRUE_COLOR = 2,
        UNCOMPRESSED_GRAYSCALE = 3,
        RLE_COLOR_MAPPED = 9,
        RLE_TRUE_COLOR = 10,
        RLE_GRAYSCALE = 11
    };

    struct __attribute__((packed)) TgaHeader {
        uint8_t id_length;
        TgaColorMapType color_map_type;
        TgaImageType image_type;
        uint16_t color_map_origin;
        uint16_t color_map_length;
        uint8_t color_map_depth;
        uint16_t x_origin;
        uint16_t y_origin;
        uint16_t width;
        uint16_t height;
        uint8_t bits_per_pixel;
        uint8_t image_descriptor;
    };

    TgaHeader read_tga_header(std::ifstream& file) {
        TgaHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(TgaHeader));
        return header;
    }

    void decode_rle(std::ifstream& file, std::vector<uint8_t>& data, int size, int bytesPerPixel) {
        int pixelCount = 0;
        while (pixelCount < size) {
            uint8_t packetHeader;
            file.read(reinterpret_cast<char*>(&packetHeader), 1);
        
            int runLength = (packetHeader & 0x7F) + 1;
            if (packetHeader & 0x80) {  // RLE packet
                std::vector<uint8_t> pixel(bytesPerPixel);
                file.read(reinterpret_cast<char*>(pixel.data()), bytesPerPixel);
                for (int i = 0; i < runLength; ++i) {
                    data.insert(data.end(), pixel.begin(), pixel.end());
                }
            } else {  // Raw packet
                int rawSize = runLength * bytesPerPixel;
                std::vector<uint8_t> rawData(rawSize);
                file.read(reinterpret_cast<char*>(rawData.data()), rawSize);
                data.insert(data.end(), rawData.begin(), rawData.end());
            }
            pixelCount += runLength;
        }
    }

    TgaImage load_tga_image(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + path);
        }

        TgaHeader header = read_tga_header(file);

        TgaImage image;
        image.width = header.width;
        image.height = header.height;
        if ((header.bits_per_pixel % 8) != 0)
            throw std::runtime_error("Cand not read a TGA image with weird bits per pixel");
        image.bytesPerPixel = header.bits_per_pixel / 8;

        int imageSize = image.width * image.height * image.bytesPerPixel;

        // Skip ID field
        file.seekg(header.id_length, std::ios::cur);

        // Handle color map if present
        std::vector<uint8_t> colorMap;
        if (header.color_map_type == TgaColorMapType::COLOR_MAP) {
            int colorMapSize = header.color_map_length * (header.color_map_depth / 8);
            colorMap.resize(colorMapSize);
            file.read(reinterpret_cast<char*>(colorMap.data()), colorMapSize);
        }

        // Read image data
        switch (header.image_type) {
            case TgaImageType::UNCOMPRESSED_COLOR_MAPPED:
            case TgaImageType::UNCOMPRESSED_TRUE_COLOR:
            case TgaImageType::UNCOMPRESSED_GRAYSCALE:
                image.data.resize(imageSize);
                file.read(reinterpret_cast<char*>(image.data.data()), imageSize);
                break;
            case TgaImageType::RLE_COLOR_MAPPED:
            case TgaImageType::RLE_TRUE_COLOR:
            case TgaImageType::RLE_GRAYSCALE:
                decode_rle(file, image.data, image.width * image.height, image.bytesPerPixel);
                break;
            default:
                std::cout << static_cast<int>(header.image_type) << "\n";
                throw std::runtime_error("Unsupported image type");
        }

        if (file.fail()) {
            throw std::runtime_error("Failed to read image data");
        }

        // Apply color map if needed
        if (header.color_map_type == TgaColorMapType::COLOR_MAP) {
            std::vector<uint8_t> mappedData;
            mappedData.reserve(image.width * image.height * (header.color_map_depth / 8));
            for (uint8_t index : image.data) {
                int colorIndex = index * (header.color_map_depth / 8);
                mappedData.insert(mappedData.end(), colorMap.begin() + colorIndex, colorMap.begin() + colorIndex + (header.color_map_depth / 8));
            }
            image.data = std::move(mappedData);
            image.bytesPerPixel = header.color_map_depth / 8;
        }

        // Convert BGR(A) to RGB(A)
        if (image.bytesPerPixel >= 3) {
            for (size_t i = 0; i < image.data.size(); i += image.bytesPerPixel) {
                std::swap(image.data[i], image.data[i + 2]);
            }
        }

        return image;
    }
} // namespace saltus::loaders::tga
