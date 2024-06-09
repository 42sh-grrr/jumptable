#include "saltus/vulkan_renderer.hh"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

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

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-function"
    static bool check_extension_is_supported(const char *name)
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
    #pragma GCC diagnostic pop

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

    static std::vector<char> read_full_file(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            throw std::runtime_error("failed to open file!");

        size_t file_size = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        return buffer;
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
        create_image_views();
        create_render_pass();
        create_graphics_pipeline();
        create_frame_buffers();
        create_command_pool_and_buffer();
        create_sync_objects();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        vkDestroySemaphore(device_, image_available_semaphore_, nullptr);
        vkDestroySemaphore(device_, render_finished_semaphore_, nullptr);
        vkDestroyFence(device_, in_flight_fence_, nullptr);
        vkDestroyCommandPool(device_, command_pool_, nullptr);
        vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
        vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
        vkDestroyRenderPass(device_, render_pass_, nullptr);
        clean_swap_chain();
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        vkDestroyDevice(device_, nullptr);
        vkDestroyInstance(instance_, nullptr);
    }

    void VulkanRenderer::render()
    {
        vkWaitForFences(device_, 1, &in_flight_fence_, VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(
            device_, swapchain_, UINT32_MAX, image_available_semaphore_,
            nullptr, &image_index
        );
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swap_chain();
            render();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("Could not acquire an image");

        vkResetFences(device_, 1, &in_flight_fence_);

        record_command_buffer(command_buffer_, image_index);

        VkSemaphore wait_semaphores[] = {
            image_available_semaphore_
        };
        VkPipelineStageFlags wait_stages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        VkSemaphore signal_semaphores[] = {
            render_finished_semaphore_
        };

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer_;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        result = vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fence_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not submit queue");

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = { swapchain_ };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &image_index;

        result = vkQueuePresentKHR(present_queue_, &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swap_chain();
            render();
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            recreate_swap_chain();
        }
        else if (result != VK_SUCCESS)
            throw std::runtime_error("Could not present to queue");
    }

    void VulkanRenderer::wait_for_idle()
    {
        vkDeviceWaitIdle(device_);
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

    VkShaderModule VulkanRenderer::create_shader_module(const std::vector<char> &code)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());
        VkShaderModule shader_module;
        VkResult result =
            vkCreateShaderModule(device_, &create_info, nullptr, &shader_module);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create shader module");
        return shader_module;
    }

    void VulkanRenderer::create_instance()
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "saltus engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
        appInfo.pEngineName = "saltus";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
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

    void VulkanRenderer::create_image_views()
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
                vkCreateImageView(device_, &create_info, nullptr, &image_view);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create an image view");
            swapchain_image_views_.push_back(image_view);
        }
    }

    void VulkanRenderer::create_render_pass()
    {
        VkAttachmentDescription color_attachment{};
        color_attachment.format = swapchain_image_format_;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;
        render_pass_info.dependencyCount = 1;
        render_pass_info.pDependencies = &dependency;

        VkResult result =
            vkCreateRenderPass(device_, &render_pass_info, nullptr, &render_pass_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create render pass");
    }

    void VulkanRenderer::create_graphics_pipeline()
    {
        auto vert_shader_code = read_full_file("saltus/shaders/shader.vert.spv");
        auto frag_shader_code = read_full_file("saltus/shaders/shader.frag.spv");

        auto vert_shader_module = create_shader_module(vert_shader_code);
        auto frag_shader_module = create_shader_module(frag_shader_code);

        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = vert_shader_module;
        vert_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = frag_shader_module;
        frag_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = {
            vert_shader_stage_info,
            frag_shader_stage_info
        };

        VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = sizeof(dynamic_states) / sizeof(*dynamic_states);
        dynamic_state.pDynamicStates = dynamic_states;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                                            | VK_COLOR_COMPONENT_G_BIT
                                            | VK_COLOR_COMPONENT_B_BIT
                                            | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        VkResult result =
            vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipeline_layout_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create pipeline layout");

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = sizeof(shader_stages) / sizeof(*shader_stages);
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertexInputInfo;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = nullptr;
        pipeline_info.pColorBlendState = &colorBlending;
        pipeline_info.pDynamicState = &dynamic_state;

        pipeline_info.layout = pipeline_layout_;
        pipeline_info.renderPass = render_pass_;
        pipeline_info.subpass = 0;

        result = vkCreateGraphicsPipelines(
            device_, nullptr, 1, &pipeline_info, nullptr, &graphics_pipeline_
        );
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create graphics pipeline");

        vkDestroyShaderModule(device_, vert_shader_module, nullptr);
        vkDestroyShaderModule(device_, frag_shader_module, nullptr);
    }

    void VulkanRenderer::create_frame_buffers()
    {
        swapchain_framebuffers_.clear();

        for (const auto &view : swapchain_image_views_)
        {
            VkFramebufferCreateInfo framebuffer_info{};
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = render_pass_;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.pAttachments = &view;
            framebuffer_info.width = swapchain_extent_.width;
            framebuffer_info.height = swapchain_extent_.height;
            framebuffer_info.layers = 1;

            VkFramebuffer framebuffer;
            VkResult result
                = vkCreateFramebuffer(device_, &framebuffer_info, nullptr, &framebuffer);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Could not create frame buffer");
            swapchain_framebuffers_.push_back(framebuffer);
        }
    }

    void VulkanRenderer::create_command_pool_and_buffer()
    {
        QueueFamilyIndices family_indices =
            get_physical_device_family_indices(physical_device_);

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = family_indices.graphicsFamily.value();

        VkResult result =
            vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create command pool");

        VkCommandBufferAllocateInfo buffer_alloc_info{};
        buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_alloc_info.commandPool = command_pool_;
        buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        buffer_alloc_info.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(device_, &buffer_alloc_info, &command_buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffer");
    }

    void VulkanRenderer::create_sync_objects()
    {
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        
        VkResult result;
        result = vkCreateSemaphore(device_, &semaphore_info, nullptr, &image_available_semaphore_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create semaphore");
        result = vkCreateSemaphore(device_, &semaphore_info, nullptr, &render_finished_semaphore_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create semaphore");
        result = vkCreateFence(device_, &fence_info, nullptr, &in_flight_fence_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create fence");
    }

    void VulkanRenderer::clean_swap_chain()
    {
        for (const auto &framebuffer : swapchain_framebuffers_)
            vkDestroyFramebuffer(device_, framebuffer, nullptr);
        for (const auto &view : swapchain_image_views_)
            vkDestroyImageView(device_, view, nullptr);
        vkDestroySwapchainKHR(device_, swapchain_, nullptr);
    }

    void VulkanRenderer::recreate_swap_chain()
    {
        vkDeviceWaitIdle(device_);

        clean_swap_chain();

        create_swap_chain();
        create_image_views();
        create_frame_buffers();
    }

    void VulkanRenderer::record_command_buffer(
        VkCommandBuffer command_buffer, uint32_t image_index
    ) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not begin command buffer");

        VkClearValue clear_color = {{{0.,0.,0.,1.}}};
        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = render_pass_;
        render_pass_begin_info.framebuffer = swapchain_framebuffers_[image_index];
        render_pass_begin_info.renderArea = {
            .offset = {0, 0},
            .extent = swapchain_extent_,
        };
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(command_buffer,
            &render_pass_begin_info,
            VK_SUBPASS_CONTENTS_INLINE
        );
        vkCmdBindPipeline(command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphics_pipeline_
        );

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapchain_extent_.width);
        viewport.height = static_cast<float>(swapchain_extent_.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain_extent_;
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        vkCmdDraw(command_buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffer_);

        result = vkEndCommandBuffer(command_buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Command buffer recording (gone wrong !!)");
    }
}
