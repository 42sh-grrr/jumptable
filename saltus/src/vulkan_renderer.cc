#include "saltus/vulkan_renderer.hh"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
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
    const std::vector<const char *> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation",
    };
    const std::vector<const char *> INSTANCE_EXTENSIONS = {
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
    };
    const std::vector<const char *> DEVICE_EXTENSIONS = {
        "VK_KHR_swapchain"
    };

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

    bool QueueFamilyIndices::is_complete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

    VulkanRenderer::VulkanRenderer(Window &window): Renderer(window)
    {
        create_instance();
        create_surface();
        choose_physical_device();
        create_device();
        create_swap_chain();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        vkDestroySwapchainKHR(device_, swapchain_, nullptr);
        vkDestroyDevice(device_, nullptr);
        vkDestroyInstance(instance_, nullptr);
    }

    QueueFamilyIndices VulkanRenderer::get_physical_device_family_indices(VkPhysicalDevice device)
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

            VkBool32 supported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface_, &supported);
            if (supported)
                indices.presentFamily = index;

            index++;
        }

        return indices;
    }

    SwapChainSupportDetails
    VulkanRenderer::get_physical_device_swap_chain_support_details(
        VkPhysicalDevice physical_device
    ) {
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

        std::vector<const char *> layers;
        if (validation_enabled_)
            layers.insert(layers.end(), VALIDATION_LAYERS.cbegin(), VALIDATION_LAYERS.cend());
        
        VkInstanceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pApplicationInfo = &appInfo;

        info.enabledLayerCount = layers.size();
        info.ppEnabledLayerNames = layers.data();

        info.enabledExtensionCount = INSTANCE_EXTENSIONS.size();
        info.ppEnabledExtensionNames = INSTANCE_EXTENSIONS.data();

        VkResult result = vkCreateInstance(&info, nullptr, &instance_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Vulkan error while creating instance");
    }

    void VulkanRenderer::create_surface()
    {
        surface_ = window_.create_vulkan_surface(instance_);
    }

    bool VulkanRenderer::is_physical_device_suitable(VkPhysicalDevice physical_device)
    {
        QueueFamilyIndices families = get_physical_device_family_indices(physical_device);
        if (!families.is_complete())
            return false;

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

        return true;
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
        QueueFamilyIndices families = get_physical_device_family_indices(physical_device_);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        
        float priority = 1.f;
        for (uint32_t index : {
            families.graphicsFamily.value(), families.presentFamily.value(),
        })
        {
            VkDeviceQueueCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = index;
            info.queueCount = 1;
            info.pQueuePriorities = &priority;
            queue_create_infos.push_back(info);
        }

        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.queueCreateInfoCount = queue_create_infos.size();
        create_info.pQueueCreateInfos = queue_create_infos.data();

        create_info.pEnabledFeatures = &device_features;

        create_info.enabledExtensionCount = DEVICE_EXTENSIONS.size();
        create_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

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
        vkGetDeviceQueue(device_, families.presentFamily.value(), 0, &present_queue_);
    }

    VkSurfaceFormatKHR VulkanRenderer::choose_swap_chain_format(
        const std::vector<VkSurfaceFormatKHR> &availableFormats
    ) {
        if (availableFormats.size() == 0)
            throw std::runtime_error("VulkanRenderer::choose_swap_chain_format was given an empty vector");
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

    VkPresentModeKHR VulkanRenderer::choose_swap_chain_present_mode(
        const std::vector<VkPresentModeKHR> &availablePresentModes
    ) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRenderer::choose_swap_extent(
        const VkSurfaceCapabilitiesKHR &capabilities
    ) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        auto window_geometry = window_.request_geometry();

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

    void VulkanRenderer::create_swap_chain()
    {
        SwapChainSupportDetails swap_chain_support =
            get_physical_device_swap_chain_support_details(physical_device_);

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
        create_info.surface = surface_;
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices =
            get_physical_device_family_indices(physical_device_);

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
            vkCreateSwapchainKHR(device_, &create_info, nullptr, &swapchain_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create swap chain");

        uint32_t real_image_count = 0;
        vkGetSwapchainImagesKHR(
            device_, swapchain_, &real_image_count, nullptr
        );
        swapchain_images_.resize(real_image_count);
        vkGetSwapchainImagesKHR(
            device_, swapchain_, &real_image_count, swapchain_images_.data()
        );
    }
}
