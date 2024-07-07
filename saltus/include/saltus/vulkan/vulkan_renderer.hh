#pragma once

#include "saltus/renderer.hh"
#include "saltus/vulkan/vulkan_device.hh"
#include "saltus/vulkan/vulkan_frame.hh"
#include "saltus/vulkan/vulkan_instance.hh"
#include "saltus/vulkan/vulkan_render_target.hh"

#include <vector>
#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    class VulkanRenderer: public Renderer
    {
    public:
        VulkanRenderer(RendererCreateInfo info);
        ~VulkanRenderer();

        const std::shared_ptr<VulkanInstance> &instance() const;
        const std::shared_ptr<VulkanDevice> &device() const;
        const std::shared_ptr<VulkanRenderTarget> &render_target() const;

        RendererPresentMode current_present_mode() const override;
        virtual void target_present_mode(RendererPresentMode) override;

        virtual matrix::Vector2<uint32_t> framebuffer_size() const override;

        void render(RenderInfo info) override;
        void wait_for_idle() override;

        std::shared_ptr<Buffer> create_buffer(BufferCreateInfo) override;
        std::shared_ptr<Shader> create_shader(ShaderCreateInfo info) override;
        std::shared_ptr<Material> create_material(MaterialCreateInfo) override;
        std::shared_ptr<Mesh> create_mesh(MeshCreateInfo) override;
        std::shared_ptr<BindGroupLayout> create_bind_group_layout(BindGroupLayoutCreateInfo) override;
        std::shared_ptr<BindGroup> create_bind_group(BindGroupCreateInfo) override;
        std::shared_ptr<InstanceGroup> create_instance_group(InstanceGroupCreateInfo) override;

    private:
        std::shared_ptr<VulkanInstance> instance_;
        std::shared_ptr<VulkanDevice> device_;
        std::shared_ptr<VulkanRenderTarget> render_target_;

        int current_frame_ = 0;
        std::vector<VulkanFrame> frames_;
    };
} // namespace saltus::vulkan
