#pragma once

#include <vulkan/vulkan_core.h>
#include "saltus/instance_group.hh"
#include "saltus/vulkan/vulkan_material.hh"
#include "saltus/vulkan/vulkan_mesh.hh"
#include "saltus/vulkan/vulkan_render_target.hh"

namespace saltus::vulkan
{
    class VulkanInstanceGroup: public InstanceGroup
    {
    public:        
        VulkanInstanceGroup(
            std::shared_ptr<VulkanRenderTarget> render_target,
            InstanceGroupCreateInfo create_info
        );
        ~VulkanInstanceGroup();

        const std::shared_ptr<VulkanRenderTarget> &render_target() const;

        const std::shared_ptr<VulkanMaterial> &material() const;
        const std::shared_ptr<VulkanMesh> &mesh() const;

        VkPipelineLayout pipeline_layout() const;
        VkPipeline pipeline() const;

        void render(VkCommandBuffer command_buffer) const;

    private:
        std::shared_ptr<VulkanRenderTarget> render_target_;

        std::shared_ptr<VulkanMaterial> material_;
        std::shared_ptr<VulkanMesh> mesh_;

        VkPipelineLayout pipeline_layout_;
        VkPipeline pipeline_;

        void create_graphics_pipeline();
        void destroy_graphics_pipeline();
    };
} // namespace saltus::vulkan
