#pragma once

#include <memory>

#include "saltus/image.hh"
#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    class VulkanImage: public Image
    {
    public:
        VulkanImage(std::shared_ptr<VulkanDevice>, ImageCreateInfo);
        ~VulkanImage();

        const std::shared_ptr<VulkanDevice> &device() const;

        const std::shared_ptr<RawVulkanImage> &raw_image() const;

        virtual void write(uint8_t *data) override;

    private:
        std::shared_ptr<VulkanDevice> device_;

        std::shared_ptr<RawVulkanImage> image_;
    };
} // namespace saltus::vulkan
