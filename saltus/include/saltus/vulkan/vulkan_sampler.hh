#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>

#include "saltus/sampler.hh"
#include "saltus/vulkan/fwd.hh"

namespace saltus::vulkan
{
    class VulkanSampler: public Sampler
    {
    public:
        VulkanSampler(std::shared_ptr<VulkanDevice>, SamplerCreateInfo);
        ~VulkanSampler();

        const std::shared_ptr<VulkanDevice> &device() const;

        const VkSampler &handle() const;
        operator VkSampler() const;

    private:
        std::shared_ptr<VulkanDevice> device_;

        VkSampler sampler_;
    };
} // namespace saltus::vulkan
