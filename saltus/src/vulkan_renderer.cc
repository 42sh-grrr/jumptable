#include "saltus/vulkan_renderer.hh"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace saltus
{
    #ifdef NDEBUG
    constexpr const bool ENABLE_VULKAN_VALIDATION = false;
    #else // NDEBUG
    constexpr const bool ENABLE_VULKAN_VALIDATION = true;
    #endif // NDEBUG
    const std::vector<const char *> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };

    bool check_extension_is_supported(const char *name)
    {
        uint32_t supported_extension_count;
        vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, nullptr);
        std::vector<VkExtensionProperties> supported_extensions(supported_extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &supported_extension_count, supported_extensions.data());

        return std::any_of(
            supported_extensions.cbegin(),
            supported_extensions.cend(),
            [&name](const VkExtensionProperties &extension) {
                return strcmp(name, extension.extensionName) == 0;
            }
        );
    }

    bool check_layer_is_supported(const char *name)
    {
        uint32_t supported_layer_count;
        vkEnumerateInstanceLayerProperties(&supported_layer_count, nullptr);
        std::vector<VkLayerProperties> supported_layers(supported_layer_count);
        vkEnumerateInstanceLayerProperties(&supported_layer_count, supported_layers.data());

        return std::any_of(
            supported_layers.cbegin(),
            supported_layers.cend(),
            [&name](const VkLayerProperties &layer) {
                return strcmp(name, layer.layerName) == 0;
            }
        );
    }

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        QueueFamilyIndices(VkPhysicalDevice device)
        {
            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
            std::vector<VkQueueFamilyProperties> families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, families.data());

            int index = 0;
            for (const auto &family : families)
            {
                if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    graphicsFamily = index;

                index++;
            }
        }

        bool is_complete()
        {
            return graphicsFamily.has_value();
        }
    };

    VulkanRenderer::VulkanRenderer()
    {
        create_instance();
        choose_physical_device();
        create_device();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        vkDestroyInstance(instance_, nullptr);
        vkDestroyDevice(device_, nullptr);
    }

    void VulkanRenderer::create_instance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "saltus";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        validation_enabled_ = ENABLE_VULKAN_VALIDATION &&
            std::all_of(
                VALIDATION_LAYERS.cbegin(), VALIDATION_LAYERS.cend(),
                check_layer_is_supported
            );

        std::vector<const char *> extensions {
            "VK_KHR_surface",
            "VK_KHR_xcb_surface",
        };

        std::vector<const char *> layers;
        if (validation_enabled_)
            layers.insert(layers.end(), VALIDATION_LAYERS.cbegin(), VALIDATION_LAYERS.cend());
        
        VkInstanceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pApplicationInfo = &appInfo;

        info.enabledLayerCount = layers.size();
        info.ppEnabledLayerNames = layers.data();

        info.enabledExtensionCount = extensions.size();
        info.ppEnabledExtensionNames = extensions.data();

        VkResult result = vkCreateInstance(&info, nullptr, &instance_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Vulkan error while creating instance");
    }

    bool VulkanRenderer::is_physical_device_suitable(VkPhysicalDevice physical_device)
    {
        QueueFamilyIndices families(physical_device);
        return families.is_complete();
    }

    void VulkanRenderer::choose_physical_device()
    {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
        if (device_count == 0)
            throw std::runtime_error("Could not find any physical device with Vulkan support");
        std::vector<VkPhysicalDevice> physical_devices(device_count);
        vkEnumeratePhysicalDevices(instance_, &device_count, physical_devices.data());

        auto found = std::find_if(
            physical_devices.cbegin(), physical_devices.cend(),
            [this](auto device) { return is_physical_device_suitable(device); }
        );
        physical_device_ = found == physical_devices.cend() ? nullptr : *found;

        if (physical_device_ == nullptr)
            throw std::runtime_error("Could not find any suitable physical device");
    }

    void VulkanRenderer::create_device()
    {
        QueueFamilyIndices families(physical_device_);

        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = families.graphicsFamily.value();
        queue_create_info.queueCount = 1;
        float priority = 1.f;
        queue_create_info.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.queueCreateInfoCount = 1;
        create_info.pQueueCreateInfos = &queue_create_info;

        create_info.pEnabledFeatures = &device_features;

        create_info.enabledExtensionCount = 0;

        if (validation_enabled_)
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
        {
            throw std::runtime_error("Vulkand device creation error");
        }

        vkGetDeviceQueue(device_, families.graphicsFamily.value(), 0, &graphics_queue_);
    }
}
