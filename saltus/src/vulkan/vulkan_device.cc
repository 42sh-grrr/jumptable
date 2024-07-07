#include "saltus/vulkan/vulkan_device.hh"
#include <algorithm>
#include <cstring>
#include <set>
#include <stdexcept>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include "logger/level.hh"
#include "saltus/vulkan/vulkan_instance.hh"

namespace saltus::vulkan
{
    const std::vector<const char *> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_VALIDATION",
    };
    const std::vector<const char *> DEVICE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };
    VkPhysicalDeviceDynamicRenderingFeaturesKHR DYNAMIC_RENDERING_FEATURES = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = nullptr,
        .dynamicRendering = true,
    };
    const VkPhysicalDeviceFeatures2 DEVICE_FEATURES2 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR,
        .pNext = &DYNAMIC_RENDERING_FEATURES,
        .features = {
        
        },
    };

    VulkanDevice::VulkanDevice(const Window &window, std::shared_ptr<VulkanInstance> instance)
        : instance_(instance), window_(window)
    {
        surface_ = window.create_vulkan_surface(instance->instance());
        
        choose_physical_device();

        QueueFamilyIndices families = get_physical_device_family_indices(physical_device_);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        float priority = 1.f;
        for (uint32_t index : std::set{
            families.graphicsFamily.value(), families.presentFamily.value(),
            families.transferFamily.value()
        })
        {
            VkDeviceQueueCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = index;
            info.queueCount = 1;
            info.pQueuePriorities = &priority;
            queue_create_infos.push_back(info);
        }

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pNext = &DEVICE_FEATURES2;

        create_info.queueCreateInfoCount = queue_create_infos.size();
        create_info.pQueueCreateInfos = queue_create_infos.data();

        create_info.enabledExtensionCount = DEVICE_EXTENSIONS.size();
        create_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        if (instance->validation_enabled())
        {
            create_info.enabledLayerCount = VALIDATION_LAYERS.size();
            create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        VkResult result = vkCreateDevice(physical_device_, &create_info, nullptr, &device_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Vulkand device creation error");

        vkGetDeviceQueue(device_, families.graphicsFamily.value(), 0, &graphics_queue_);
        vkGetDeviceQueue(device_, families.presentFamily.value(), 0, &present_queue_);
        vkGetDeviceQueue(device_, families.transferFamily.value(), 0, &transfer_queue_);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = families.graphicsFamily.value();

        result = vkCreateCommandPool(device_, &pool_info, nullptr, &resettable_command_buffer_pool_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create command pool");

        pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        result = vkCreateCommandPool(device_, &pool_info, nullptr, &transient_command_buffer_pool_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create command pool");
    }

    VulkanDevice::~VulkanDevice()
    {
        vkDestroyCommandPool(device_, resettable_command_buffer_pool_, nullptr);
        vkDestroyCommandPool(device_, transient_command_buffer_pool_, nullptr);

        vkDestroySurfaceKHR(*instance_, surface_, nullptr);
        vkDestroyDevice(device_, nullptr);
    }

    VulkanDevice::operator VkDevice() const
    {
        return device_;
    }

    const Window &VulkanDevice::window() const
    {
        return window_;
    }

    QueueFamilyIndices VulkanDevice::get_physical_device_family_indices() const
    {
        if (physical_device_ == nullptr)
            throw std::runtime_error("Physical device has not been yet set");
        return get_physical_device_family_indices(physical_device_);
    }

    QueueFamilyIndices VulkanDevice::get_physical_device_family_indices(VkPhysicalDevice device) const
    {
        QueueFamilyIndices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

        int index = 0;
        for (const auto &family : families)
        {
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = index;
            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 && (family.queueFlags & VK_QUEUE_TRANSFER_BIT))
                indices.transferFamily = index;

            VkBool32 supported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface_, &supported);
            if (supported)
                indices.presentFamily = index;

            index++;
        }
        // graphics family is a valid transfer family
        if (!indices.transferFamily.has_value())
            indices.transferFamily = indices.graphicsFamily;

        return indices;
    }

    SwapChainSupportDetails
    VulkanDevice::get_physical_device_swap_chain_support_details() const {
        return get_physical_device_swap_chain_support_details(physical_device_);
    }

    SwapChainSupportDetails VulkanDevice::get_physical_device_swap_chain_support_details(
        VkPhysicalDevice physical_device
    ) const {
        SwapChainSupportDetails details{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical_device, surface_, &details.capabilities
        );

        uint32_t surface_format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device, surface_, &surface_format_count, nullptr
        );
        if (surface_format_count != 0)
        {
            details.formats.resize(surface_format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                physical_device, surface_, &surface_format_count, details.formats.data()
            );
        }

        uint32_t surface_present_modes_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device, surface_, &surface_present_modes_count, nullptr
        );
        if (surface_present_modes_count != 0)
        {
            details.present_modes.resize(surface_present_modes_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                physical_device, surface_, &surface_present_modes_count,
                details.present_modes.data()
            );
        }

        return details;
    }
    
    VkSurfaceKHR VulkanDevice::surface() const
    {
        return surface_;
    }

    VkPhysicalDevice VulkanDevice::physical_device() const
    {
        return physical_device_;
    }

    VkDevice VulkanDevice::device() const
    {
        return device_;
    }

    VkQueue VulkanDevice::graphics_queue() const
    {
        return graphics_queue_;
    }

    VkQueue VulkanDevice::present_queue() const
    {
        return present_queue_;
    }

    VkQueue VulkanDevice::transfer_queue() const
    {
        return transfer_queue_;
    }

    VkCommandPool VulkanDevice::resettable_command_buffer_pool() const
    {
        return resettable_command_buffer_pool_;
    }

    VkCommandPool VulkanDevice::transient_command_buffer_pool() const
    {
        return transient_command_buffer_pool_;
    }

    bool VulkanDevice::is_physical_device_suitable(VkPhysicalDevice physical_device)
    {
        // Check for complete queue families support

        QueueFamilyIndices families = get_physical_device_family_indices(physical_device);
        if (!families.is_complete())
            return false;

        // Check for extensions support

        uint32_t extensions_count = 0;
        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &extensions_count, nullptr
        );
        std::vector<VkExtensionProperties> extensions(extensions_count);
        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &extensions_count, extensions.data()
        );

        bool support_extensions = std::all_of(
            DEVICE_EXTENSIONS.cbegin(), DEVICE_EXTENSIONS.cend(),
            [extensions](const char *required) {
                return std::any_of(
                    extensions.cbegin(), extensions.cend(),
                    [required](const VkExtensionProperties &extension) {
                        return strcmp(extension.extensionName, required) == 0;
                    }
                );
            }
        );
        if (!support_extensions)
            return false;

        SwapChainSupportDetails swapchain_support =
            get_physical_device_swap_chain_support_details(physical_device);
        if (swapchain_support.formats.empty() || swapchain_support.present_modes.empty())
            return false;

        // lol

        VkPhysicalDeviceProperties device_properties{};
        vkGetPhysicalDeviceProperties(physical_device, &device_properties);
        // TODO: Remove lol
        if (strcmp(device_properties.deviceName, "NVIDIA GeForce GTX 1650 Ti") == 0)
            return false;
        logger::debug() << "Using device '" << device_properties.deviceName << "' (" << string_VkPhysicalDeviceType(device_properties.deviceType) << ")\n";

        // Check for dynamic rendering support

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features = {};
        dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;

        VkPhysicalDeviceFeatures2 device_features2 = {};
        device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        device_features2.pNext = &dynamic_rendering_features;

        vkGetPhysicalDeviceFeatures2(physical_device, &device_features2);

        if (!dynamic_rendering_features.dynamicRendering)
            return false;

        return true;
    }

    void VulkanDevice::choose_physical_device()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance_->instance(), &device_count, nullptr);
        if (device_count == 0)
            throw std::runtime_error("Could not find any physical device with Vulkan support");
        std::vector<VkPhysicalDevice> physical_devices(device_count);
        vkEnumeratePhysicalDevices(instance_->instance(), &device_count, physical_devices.data());

        auto found = std::find_if(
            physical_devices.cbegin(), physical_devices.cend(),
            [this](auto device) { return is_physical_device_suitable(device); }
        );
        physical_device_ = found == physical_devices.cend() ? nullptr : *found;

        if (physical_device_ == nullptr)
            throw std::runtime_error("Could not find any suitable physical device");
    }
} // namespace saltus::vulkan
