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

        VkDescriptorSetLayout descriptor_set_layout_;
        VkPipelineLayout pipeline_layout_;
        VkPipeline pipeline_;

        /// Setup while creating the pipeline
        /// The buffers are owned by meshes, this is just used for
        /// quick binding of all vertex buffers during render
        std::vector<VkBuffer> vertex_buffers_;
        /// see vertex_buffers_
        std::vector<VkDeviceSize> vertex_offsets_;

        void create_descriptor_set_layout();
        void create_pipeline_layout();
        void create_pipeline();
        void destroy_descriptor_set_layout();
        void destroy_pipeline_layout();
        void destroy_pipeline();
    };
} // namespace saltus::vulkan
