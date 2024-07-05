#include "saltus/vulkan/vulkan_renderer.hh"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>
#include <saltus/vulkan/vulkan_shader.hh>
#include "saltus/vulkan/vulkan_bind_group_layout.hh"
#include "saltus/vulkan/vulkan_bind_group.hh"
#include "saltus/vulkan/vulkan_material.hh"
#include "saltus/vulkan/vulkan_mesh.hh"
#include "saltus/vulkan/vulkan_render_target.hh"
#include "saltus/vulkan/vulkan_instance_group.hh"

namespace saltus::vulkan
{
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
        return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
    }

    VulkanRenderer::VulkanRenderer(Window &window): Renderer(window)
    {
        instance_ = std::make_shared<VulkanInstance>();
        device_ = std::make_shared<VulkanDevice>(window, instance_);
        render_target_ = std::make_shared<VulkanRenderTarget>(device_);
        
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
    }

    void VulkanRenderer::render(const RenderInfo info)
    {
        render_target_->resize_if_changed();

        auto device = device_->device();

        vkWaitForFences(device, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);
        uint32_t image_index =
            render_target_->acquire_next_image(image_available_semaphores_[current_frame_]);
        vkResetFences(device, 1, &in_flight_fences_[current_frame_]);

        record_command_buffer(command_buffers_[current_frame_], image_index, info);

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
            render(info);
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

    std::shared_ptr<Buffer> VulkanRenderer::create_buffer(BufferCreateInfo info)
    {
        return std::make_shared<VulkanBuffer>(device_, info);
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

    std::shared_ptr<BindGroupLayout> VulkanRenderer::create_bind_group_layout(BindGroupLayoutCreateInfo info)
    {
        return std::make_shared<VulkanBindGroupLayout>(device_, info);
    }

    std::shared_ptr<BindGroup> VulkanRenderer::create_bind_group(BindGroupCreateInfo info)
    {
        return std::make_shared<VulkanBindGroup>(device_, info);
    }

    std::shared_ptr<InstanceGroup> VulkanRenderer::create_instance_group(InstanceGroupCreateInfo info)
    {
        return std::make_shared<VulkanInstanceGroup>(render_target_, info);
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
        VkCommandBuffer command_buffer, uint32_t image_index,
        const RenderInfo &info
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
        for (const auto &instance_group : info.instance_groups)
        {
            if (const auto &vk_instance_group = std::dynamic_pointer_cast<VulkanInstanceGroup>(instance_group))
            {
                vk_instance_group->render(command_buffer);
            }
        }
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
