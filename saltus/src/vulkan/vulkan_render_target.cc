#include "saltus/vulkan/vulkan_render_target.hh"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    VulkanRenderTarget::VulkanRenderTarget(std::shared_ptr<VulkanDevice> device):
        device_(device)
    {
        create();
    }

    VulkanRenderTarget::~VulkanRenderTarget()
    {
        destroy();
    }

    const std::shared_ptr<VulkanDevice> &VulkanRenderTarget::device() const
    {
        return device_;
    }

    const VkFormat &VulkanRenderTarget::swapchain_image_format() const
    {
        return swapchain_image_format_;
    }
    const VkExtent2D &VulkanRenderTarget::swapchain_extent() const
    {
        return swapchain_extent_;
    }
    const VkSwapchainKHR &VulkanRenderTarget::swapchain() const
    {
        return swapchain_;
    }
    const std::vector<VkImage> &VulkanRenderTarget::swapchain_images() const
    {
        return swapchain_images_;
    }
    const std::vector<VkImageView> &VulkanRenderTarget::swapchain_image_views() const
    {
        return swapchain_image_views_;
    }

    void VulkanRenderTarget::resize_if_changed()
    {
        SwapChainSupportDetails swap_chain_support =
            device_->get_physical_device_swap_chain_support_details();
        VkExtent2D new_extent =
            choose_swap_extent(swap_chain_support.capabilities);
        if (new_extent.width != swapchain_extent_.width || new_extent.height != swapchain_extent_.height)
        {
            recreate();
        }
    }
    
    void VulkanRenderTarget::recreate()
    {
        vkDeviceWaitIdle(*device_);
        destroy();
        create();
    }

    VkSurfaceFormatKHR VulkanRenderTarget::choose_swap_chain_format(
        const std::vector<VkSurfaceFormatKHR> &availableFormats
    ) {
        if (availableFormats.size() == 0)
            throw std::runtime_error("VulkanRenderTarget::choose_swap_chain_format was given an empty vector");
        for (const auto &format : availableFormats)
        {
            if (
                format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
            ) {
                return format;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanRenderTarget::choose_swap_chain_present_mode(
        const std::vector<VkPresentModeKHR> &availablePresentModes
    ) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRenderTarget::choose_swap_extent(
        const VkSurfaceCapabilitiesKHR &capabilities
    ) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        auto window_geometry = device_->window().request_geometry();

        uint32_t target_width = std::clamp(
            static_cast<uint32_t>(window_geometry.width),
            capabilities.minImageExtent.width, capabilities.maxImageExtent.width
        );
        uint32_t target_height = std::clamp(
            static_cast<uint32_t>(window_geometry.height),
            capabilities.minImageExtent.height, capabilities.maxImageExtent.height
        );

        return VkExtent2D {
            .width = target_width,
            .height = target_height,
        };
    }

    void VulkanRenderTarget::create()
    {
        create_swap_chain();
        create_images();
        create_image_views();
    }

    uint32_t VulkanRenderTarget::acquire_next_image(
        VkSemaphore semaphore,
        VkFence fence
    ) {
        uint32_t index = 0;
        VkResult result = vkAcquireNextImageKHR(
            *device_, swapchain_, UINT32_MAX, semaphore, fence, &index
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate();
            // try again
            return acquire_next_image(semaphore, fence);
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("Could not acquire an image");
        return index;
    }

    void VulkanRenderTarget::create_swap_chain()
    {
        SwapChainSupportDetails swap_chain_support =
            device_->get_physical_device_swap_chain_support_details();

        VkSurfaceFormatKHR surface_format =
            choose_swap_chain_format(swap_chain_support.formats);
        swapchain_image_format_ = surface_format.format;
        VkPresentModeKHR present_mode =
            choose_swap_chain_present_mode(swap_chain_support.present_modes);
        VkExtent2D extent =
            choose_swap_extent(swap_chain_support.capabilities);
        swapchain_extent_ = extent;

        uint32_t max_image_count = swap_chain_support.capabilities.maxImageCount;
        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
        if (max_image_count != 0 && image_count > max_image_count)
            image_count = max_image_count;

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = device_->surface();
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = device_->get_physical_device_family_indices();

        uint32_t queue_family_indices[] = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
        };

        if (indices.graphicsFamily != indices.presentFamily) {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount =
                sizeof(queue_family_indices) / sizeof(*queue_family_indices);
            create_info.pQueueFamilyIndices = queue_family_indices;
        } else {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0; // Optional
            create_info.pQueueFamilyIndices = nullptr; // Optional
        }

        create_info.preTransform = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        VkResult result =
            vkCreateSwapchainKHR(device_->device(), &create_info, nullptr, &swapchain_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create swap chain");
    }

    void VulkanRenderTarget::create_images()
    {
        uint32_t real_image_count = 0;
        vkGetSwapchainImagesKHR(
            *device_, swapchain_, &real_image_count, nullptr
        );
        swapchain_images_.resize(real_image_count);
        vkGetSwapchainImagesKHR(
            *device_, swapchain_, &real_image_count, swapchain_images_.data()
        );
    }

    void VulkanRenderTarget::create_image_views()
    {
        swapchain_image_views_.clear();

        for (const auto &image : swapchain_images_)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = image;
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = swapchain_image_format_;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;
            VkImageView image_view;
            VkResult result =
                vkCreateImageView(*device_, &create_info, nullptr, &image_view);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create an image view");
            swapchain_image_views_.push_back(image_view);
        }
    }
    
    void VulkanRenderTarget::destroy()
    {
        destroy_image_views();
        swapchain_image_views_.clear();
        destroy_swap_chain();
        swapchain_images_.clear();
        swapchain_ = VK_NULL_HANDLE;
    }

    void VulkanRenderTarget::destroy_swap_chain()
    {
        vkDestroySwapchainKHR(*device_, swapchain_, nullptr);
    }

    void VulkanRenderTarget::destroy_image_views()
    {
        for (const auto &view : swapchain_image_views_)
            vkDestroyImageView(*device_, view, nullptr);
    }
} // namespace saltus::vulkan

