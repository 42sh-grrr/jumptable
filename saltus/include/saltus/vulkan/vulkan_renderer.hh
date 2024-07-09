#pragma once

#include "saltus/image.hh"
#include "saltus/renderer.hh"

#include <vulkan/vulkan_core.h>

namespace saltus::vulkan
{
    class VulkanInstance;
    class VulkanDevice;
    class VulkanRenderTarget;
    class FrameRing;

    class VulkanRenderer: public Renderer
    {
    public:
        VulkanRenderer(RendererCreateInfo info);
        ~VulkanRenderer();

        const std::shared_ptr<VulkanInstance> &instance() const;
        const std::shared_ptr<VulkanDevice> &device() const;
        const std::shared_ptr<VulkanRenderTarget> &render_target() const;
        const std::shared_ptr<FrameRing> &frame_ring() const;

        RendererPresentMode current_present_mode() const override;
        virtual void target_present_mode(RendererPresentMode) override;

        virtual matrix::Vector2<uint32_t> framebuffer_size() const override;

        void render(RenderInfo info) override;
        void wait_for_idle() override;

        std::shared_ptr<Buffer> create_buffer(BufferCreateInfo) override;
        std::shared_ptr<Image> create_image(ImageCreateInfo) override;
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
        std::shared_ptr<FrameRing> frame_ring_;
    };
} // namespace saltus::vulkan
