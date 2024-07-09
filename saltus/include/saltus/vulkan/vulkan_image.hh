#pragma once

#include "saltus/image.hh"
#include "saltus/vulkan/raw_vulkan_image.hh"
#include "saltus/vulkan/vulkan_device.hh"

namespace saltus::vulkan
{
    class VulkanImage: public Image
    {
    public:
        VulkanImage(std::shared_ptr<VulkanDevice>, ImageCreateInfo);
        ~VulkanImage();

        const std::shared_ptr<VulkanDevice> &device() const;

        RawVulkanImage &image();
        const RawVulkanImage &image() const;

        virtual void write(uint8_t *data) override;

    private:
        std::shared_ptr<VulkanDevice> device_;

        std::unique_ptr<RawVulkanImage> image_;
    };
} // namespace saltus::vulkan
