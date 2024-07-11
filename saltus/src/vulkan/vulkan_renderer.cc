#include "saltus/vulkan/vulkan_renderer.hh"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>
#include <logger/level.hh>

#include "matrix/vector.hh"
#include "saltus/vulkan/vulkan_image.hh"
#include "saltus/vulkan/vulkan_sampler.hh"
#include "saltus/vulkan/vulkan_shader.hh"
#include "saltus/vulkan/config.hh"
#include "saltus/vulkan/vulkan_bind_group_layout.hh"
#include "saltus/vulkan/vulkan_bind_group.hh"
#include "saltus/vulkan/vulkan_material.hh"
#include "saltus/vulkan/vulkan_mesh.hh"
#include "saltus/vulkan/vulkan_render_target.hh"
#include "saltus/vulkan/vulkan_instance_group.hh"
#include "saltus/vulkan/frame_ring.hh"
#include "saltus/vulkan/vulkan_texture.hh"

namespace saltus::vulkan
{
    bool QueueFamilyIndices::is_complete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
    }

    VulkanRenderer::VulkanRenderer(RendererCreateInfo info): Renderer(info)
    {
        frame_ring_ = std::make_shared<FrameRing>(*this);

        instance_ = std::make_shared<VulkanInstance>();
        device_ = std::make_shared<VulkanDevice>(info.window, instance_);
        render_target_ = std::make_shared<VulkanRenderTarget>(
            frame_ring_, device_, info.target_present_mode, info.msaa_samples
        );

        frame_ring_->resize(MAX_FRAMES_IN_FLIGHT);
    }

    VulkanRenderer::~VulkanRenderer()
    { }

    const std::shared_ptr<VulkanInstance> &VulkanRenderer::instance() const
    {
        return instance_;
    }

    const std::shared_ptr<VulkanDevice> &VulkanRenderer::device() const
    {
        return device_;
    }

    const std::shared_ptr<VulkanRenderTarget> &VulkanRenderer::render_target() const
    {
        return render_target_;
    }

    const std::shared_ptr<FrameRing> &VulkanRenderer::frame_ring() const
    {
        return frame_ring_;
    }

    RendererPresentMode VulkanRenderer::current_present_mode() const
    {
        return vulkan_present_mode_to_renderer_present_mode(render_target_->present_mode());
    }

    void VulkanRenderer::target_present_mode(RendererPresentMode newmode)
    {
        render_target_->target_present_mode(newmode);
        Renderer::target_present_mode(newmode);
    }

    matrix::Vector2<uint32_t> VulkanRenderer::framebuffer_size() const
    {
        auto extent = render_target_->swapchain_extent();
        return matrix::Vector2<uint32_t>({ extent.width, extent.height });
    }

    void VulkanRenderer::render(const RenderInfo info)
    {
        render_target_->resize_if_changed();

        auto device = device_->device();
        auto &frame = frame_ring_->frame();

        vkWaitForFences(device, 1, &frame.in_flight_fence(), VK_TRUE, UINT64_MAX);
        uint32_t image_index =
            render_target_->acquire_next_image(frame.image_available_semaphore());
        vkResetFences(device, 1, &frame.in_flight_fence());

        frame.record(info, image_index);

        std::array wait_semaphores {
            frame.image_available_semaphore()
        };
        std::array<VkPipelineStageFlags, 1> wait_stages = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        std::array signal_semaphores {
            frame.render_finished_semaphore()
        };

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = wait_semaphores.data();
        submit_info.pWaitDstStageMask = wait_stages.data();
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &frame.command_buffer();
        submit_info.signalSemaphoreCount = signal_semaphores.size();
        submit_info.pSignalSemaphores = signal_semaphores.data();

        VkResult result = vkQueueSubmit(device_->graphics_queue(), 1, &submit_info, frame.in_flight_fence());
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not submit queue");

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        present_info.waitSemaphoreCount = signal_semaphores.size();
        present_info.pWaitSemaphores = signal_semaphores.data();

        std::array<VkSwapchainKHR, 1> swapchains { render_target_->swapchain() };
        present_info.swapchainCount = swapchains.size();
        present_info.pSwapchains = swapchains.data();
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

        frame_ring_->next_frame();
    }

    void VulkanRenderer::wait_for_idle()
    {
        vkDeviceWaitIdle(device_->device());
    }

    std::shared_ptr<Buffer> VulkanRenderer::create_buffer(BufferCreateInfo info)
    {
        return std::make_shared<VulkanBuffer>(device_, info);
    }

    std::shared_ptr<Image> VulkanRenderer::create_image(ImageCreateInfo info)
    {
        return std::make_shared<VulkanImage>(device_, info);
    }

    std::shared_ptr<Sampler> VulkanRenderer::create_sampler(SamplerCreateInfo info)
    {
        return std::make_shared<VulkanSampler>(device_, info);
    }

    std::shared_ptr<Texture> VulkanRenderer::create_texture(TextureCreateInfo info)
    {
        return std::make_shared<VulkanTexture>(device_, info);
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
}
