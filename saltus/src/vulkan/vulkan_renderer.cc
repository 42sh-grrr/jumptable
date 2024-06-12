#include "saltus/vulkan/vulkan_renderer.hh"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>
#include <saltus/vulkan/vulkan_shader.hh>
#include "saltus/vulkan/vulkan_material.hh"
#include "saltus/vulkan/vulkan_mesh.hh"
#include "saltus/vulkan/vulkan_render_target.hh"

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
    const std::vector<const char *> DEVICE_EXTENSIONS = {
        "VK_KHR_swapchain"
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

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
        instance_ = std::make_shared<VulkanInstance>();
        device_ = std::make_shared<VulkanDevice>(window, instance_);
        render_target_ = std::make_shared<VulkanRenderTarget>(device_);
        
        create_graphics_pipeline();
        create_command_pool_and_buffers();
        create_sync_objects();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        auto device = device_->device();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, render_finished_semaphores_[i], nullptr);
            vkDestroySemaphore(device, image_available_semaphores_[i], nullptr);
            vkDestroyFence(device, in_flight_fences_[i], nullptr);
        }
        vkDestroyCommandPool(device, command_pool_, nullptr);
        vkDestroyPipeline(device, graphics_pipeline_, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout_, nullptr);
    }

    void VulkanRenderer::render()
    {
        auto device = device_->device();

        vkWaitForFences(device, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);
        uint32_t image_index =
            render_target_->acquire_next_image(image_available_semaphores_[current_frame_]);
        vkResetFences(device, 1, &in_flight_fences_[current_frame_]);

        record_command_buffer(command_buffers_[current_frame_], image_index);

        VkSemaphore wait_semaphores[] = {
            image_available_semaphores_[current_frame_]
        };
        VkPipelineStageFlags wait_stages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        VkSemaphore signal_semaphores[] = {
            render_finished_semaphores_[current_frame_]
        };

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffers_[current_frame_];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;

        VkResult result = vkQueueSubmit(device_->graphics_queue(), 1, &submit_info, in_flight_fences_[current_frame_]);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not submit queue");

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = signal_semaphores;

        VkSwapchainKHR swapchains[] = { render_target_->swapchain() };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &image_index;

        result = vkQueuePresentKHR(device_->present_queue(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            render_target_->recreate();
            render();
        }
        else if (result == VK_SUBOPTIMAL_KHR)
        {
            render_target_->recreate();
        }
        else if (result != VK_SUCCESS)
            throw std::runtime_error("Could not present to queue");

        current_frame_ = (current_frame_+1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::wait_for_idle()
    {
        vkDeviceWaitIdle(device_->device());
    }

    std::shared_ptr<Shader> VulkanRenderer::create_shader(ShaderCreateInfo info)
    {
        return std::make_shared<VulkanShader>(device_, info);
    }

    std::shared_ptr<Material> VulkanRenderer::create_material(MaterialCreateInfo info)
    {
        return std::make_shared<VulkanMaterial>(device_, info);
    }

    std::shared_ptr<Mesh> VulkanRenderer::create_mesh(MeshCreateInfo info)
    {
        return std::make_shared<VulkanMesh>(device_, info);
    }


    void VulkanRenderer::create_graphics_pipeline()
    {
        VulkanShader vert_shader (device_, {
            .source_code = read_full_file("saltus/shaders/shader.vert.spv"),
        });
        VulkanShader frag_shader (device_, {
            .source_code = read_full_file("saltus/shaders/shader.frag.spv"),
        });

        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = vert_shader.module();
        vert_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = frag_shader.module();
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
            vkCreatePipelineLayout(*device_, &pipelineLayoutInfo, nullptr, &pipeline_layout_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create pipeline layout");

        VkPipelineRenderingCreateInfoKHR pipeline_create{VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
        pipeline_create.colorAttachmentCount    = 1;
        pipeline_create.pColorAttachmentFormats = &render_target_->swapchain_image_format();
        // TODO: Depth buffer
        // pipeline_create.depthAttachmentFormat   = render_target_->swapchain_image_format();
        // pipeline_create.stencilAttachmentFormat = render_target_->swapchain_image_format();

        VkGraphicsPipelineCreateInfo pipeline_info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipeline_info.pNext = &pipeline_create;
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
        pipeline_info.renderPass = nullptr;
        pipeline_info.subpass = 0;

        result = vkCreateGraphicsPipelines(
            *device_, nullptr, 1, &pipeline_info, nullptr, &graphics_pipeline_
        );
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create graphics pipeline");
    }

    void VulkanRenderer::create_command_pool_and_buffers()
    {
        QueueFamilyIndices family_indices =
            device_->get_physical_device_family_indices();

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = family_indices.graphicsFamily.value();

        VkResult result =
            vkCreateCommandPool(*device_, &pool_info, nullptr, &command_pool_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create command pool");

        VkCommandBufferAllocateInfo buffer_alloc_info{};
        buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_alloc_info.commandPool = command_pool_;
        buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        buffer_alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

        command_buffers_.resize(MAX_FRAMES_IN_FLIGHT);

        result = vkAllocateCommandBuffers(*device_, &buffer_alloc_info, command_buffers_.data());
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

        image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
        render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
        in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkResult result;
            result = vkCreateSemaphore(*device_, &semaphore_info, nullptr, &image_available_semaphores_[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Could not create semaphore");
            result = vkCreateSemaphore(*device_, &semaphore_info, nullptr, &render_finished_semaphores_[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Could not create semaphore");
            result = vkCreateFence(*device_, &fence_info, nullptr, &in_flight_fences_[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Could not create fence");
        }
    }

    void VulkanRenderer::record_command_buffer(
        VkCommandBuffer command_buffer, uint32_t image_index
    ) {
        auto image_view = render_target_->swapchain_image_views()[image_index];
        auto image = render_target_->swapchain_images()[image_index];

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkResult result = vkBeginCommandBuffer(command_buffer, &begin_info);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not begin command buffer");

        // <> Prepare render image layout for rendering

        const VkImageMemoryBarrier image_memory_barrier_init {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
            },
        };

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1,
            &image_memory_barrier_init
        );

        // </> Prepare render image layout for rendering

        VkRenderingInfo rendering_info{};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        rendering_info.renderArea = {
            .offset = { 0, 0 },
            .extent = render_target_->swapchain_extent(),
        };
        rendering_info.layerCount = 1;

        VkRenderingAttachmentInfo color_attachment { };
        color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        color_attachment.imageView = image_view;
        color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        VkClearValue clear_color = {{{0.,0.,0.,1.}}};
        color_attachment.clearValue = clear_color;

        VkRenderingAttachmentInfo attachments[] = { color_attachment };
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = attachments;

        vkCmdBeginRendering(command_buffer, &rendering_info);
        vkCmdBindPipeline(command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphics_pipeline_
        );

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(render_target_->swapchain_extent().width);
        viewport.height = static_cast<float>(render_target_->swapchain_extent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = render_target_->swapchain_extent();
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        vkCmdDraw(command_buffer, 3, 1, 0, 0);

        vkCmdEndRendering(command_buffer);

        // <> Prepare render image layout for presentation

        const VkImageMemoryBarrier image_memory_barrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
            }
        };

        vkCmdPipelineBarrier(
            command_buffer,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1,
            &image_memory_barrier
        );

        // <p> Prepare render image layout for presentation

        result = vkEndCommandBuffer(command_buffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Command buffer recording (gone wrong !!)");
    }
}
