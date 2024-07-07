#include "saltus/vulkan/vulkan_frame.hh"

#include <vulkan/vulkan_core.h>

#include "saltus/renderer.hh"
#include "saltus/vulkan/vulkan_instance_group.hh"

namespace saltus::vulkan
{
    VulkanFrame::VulkanFrame(std::shared_ptr<VulkanRenderTarget> render_target, uint32_t frame_index):
        render_target_(render_target), frame_index_(frame_index)
    {
        auto &device = render_target_->device();

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkResult result;
        result = vkCreateSemaphore(*device, &semaphore_info, nullptr, &image_available_semaphore_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create semaphore");
        result = vkCreateSemaphore(*device, &semaphore_info, nullptr, &render_finished_semaphore_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create semaphore");
        result = vkCreateFence(*device, &fence_info, nullptr, &in_flight_fence_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create fence");

        VkCommandBufferAllocateInfo buffer_alloc_info{};
        buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_alloc_info.commandPool = device->resettable_command_buffer_pool();
        buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        buffer_alloc_info.commandBufferCount = 1;

        result = vkAllocateCommandBuffers(*device, &buffer_alloc_info, &command_buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffer");
    }

    VulkanFrame::VulkanFrame(VulkanFrame&& other):
        render_target_(std::move(other.render_target_)), frame_index_(other.frame_index_)
    {
        command_buffer_ = other.command_buffer_;
        other.command_buffer_ = VK_NULL_HANDLE;

        image_available_semaphore_ = other.image_available_semaphore_;
        other.image_available_semaphore_ = VK_NULL_HANDLE;

        render_finished_semaphore_ = other.render_finished_semaphore_;
        other.render_finished_semaphore_ = VK_NULL_HANDLE;

        in_flight_fence_ = other.in_flight_fence_;
        other.in_flight_fence_ = VK_NULL_HANDLE;
    }

    VulkanFrame::~VulkanFrame()
    {
        if (!render_target_)
            return;

        auto &device = render_target_->device();

        if (image_available_semaphore_ != VK_NULL_HANDLE)
            vkDestroySemaphore(*device, image_available_semaphore_, nullptr);
        if (render_finished_semaphore_ != VK_NULL_HANDLE)
            vkDestroySemaphore(*device, render_finished_semaphore_, nullptr);
        if (in_flight_fence_ != VK_NULL_HANDLE)
            vkDestroyFence(*device, in_flight_fence_, nullptr);
    }

    const VkCommandBuffer &VulkanFrame::command_buffer() const
    {
        return command_buffer_;
    }

    const VkSemaphore &VulkanFrame::image_available_semaphore() const
    {
        return image_available_semaphore_;
    }

    const VkSemaphore &VulkanFrame::render_finished_semaphore() const
    {
        return render_finished_semaphore_;
    }

    const VkFence &VulkanFrame::in_flight_fence() const
    {
        return in_flight_fence_;
    }

    void VulkanFrame::record(const RenderInfo &info, uint32_t image_index)
    {
        auto image_view = render_target_->swapchain_image_views()[image_index];
        auto image = render_target_->swapchain_images()[image_index];

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkResult result = vkBeginCommandBuffer(command_buffer_, &begin_info);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not begin command buffer");

        // <> Prepare render image layout for rendering

        const VkImageMemoryBarrier image_memory_barrier_init {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_NONE,
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
            command_buffer_,
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
        color_attachment.clearValue = VkClearValue {
            .color = {.float32{
                info.clear_color.x(),
                info.clear_color.y(),
                info.clear_color.z(),
                info.clear_color.w(),
            }},
        };

        VkRenderingAttachmentInfo attachments[] = { color_attachment };
        rendering_info.colorAttachmentCount = 1;
        rendering_info.pColorAttachments = attachments;

        vkCmdBeginRendering(command_buffer_, &rendering_info);
        for (const auto &instance_group : info.instance_groups)
        {
            if (const auto &vk_instance_group = std::dynamic_pointer_cast<VulkanInstanceGroup>(instance_group))
            {
                vk_instance_group->render(command_buffer_);
            }
        }
        vkCmdEndRendering(command_buffer_);

        // <> Prepare render image layout for presentation

        const VkImageMemoryBarrier image_memory_barrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_NONE,
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
            command_buffer_,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1,
            &image_memory_barrier
        );

        // </> Prepare render image layout for presentation

        result = vkEndCommandBuffer(command_buffer_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Command buffer recording (gone wrong !!)");
    }
} // namespace saltus::vulkan


