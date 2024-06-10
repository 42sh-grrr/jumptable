#include "saltus/vulkan/vulkan_instance.hh"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>
#include <saltus/vulkan/vulkan_shader.hh>

namespace saltus::vulkan
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

    static bool check_layer_is_supported(const char *name)
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

    VulkanInstance::VulkanInstance(): VulkanInstance(ENABLE_VULKAN_VALIDATION)
    { }

    VulkanInstance::VulkanInstance(bool enable_validation)
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "saltus engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.pEngineName = "saltus";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        validation_enabled_ = enable_validation &&
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

    VulkanInstance::operator VkInstance() const
    {
        return instance_;
    }

    bool VulkanInstance::validation_enabled()
    {
        return validation_enabled_;
    }

    VkInstance VulkanInstance::instance()
    {
        return instance_;
    }
}

