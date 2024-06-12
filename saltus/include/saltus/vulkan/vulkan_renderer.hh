#pragma once

#include "saltus/renderer.hh"
#include "saltus/vulkan/vulkan_device.hh"
#include "saltus/vulkan/vulkan_instance.hh"
#include "saltus/vulkan/vulkan_render_target.hh"

#include <vector>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    class VulkanRenderer: public Renderer
    {
    public:
        VulkanRenderer(Window &window);
        ~VulkanRenderer();

        void render() override;
        void wait_for_idle() override;

        std::shared_ptr<Shader> create_shader(ShaderCreateInfo info) override;
        std::shared_ptr<Material> create_material(MaterialCreateInfo) override;
        std::shared_ptr<Mesh> create_mesh(MeshCreateInfo) override;

    private:
        std::shared_ptr<VulkanInstance> instance_;
        std::shared_ptr<VulkanDevice> device_;
        std::shared_ptr<VulkanRenderTarget> render_target_;

        VkPipelineLayout pipeline_layout_;
        VkPipeline graphics_pipeline_;

        VkCommandPool command_pool_;
        std::vector<VkCommandBuffer> command_buffers_;

        std::vector<VkSemaphore> image_available_semaphores_;
        std::vector<VkSemaphore> render_finished_semaphores_;
        std::vector<VkFence> in_flight_fences_;

        int current_frame_ = 0;

        void create_graphics_pipeline();
        void create_command_pool_and_buffers();
        void create_sync_objects();

        void record_command_buffer(
            VkCommandBuffer command_buffer, uint32_t image_index
        );
    };
}
