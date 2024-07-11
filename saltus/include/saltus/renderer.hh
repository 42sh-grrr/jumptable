#pragma once

#include <memory>

#include "matrix/vector.hh"
#include "saltus/bind_group.hh"
#include "saltus/image.hh"
#include "saltus/instance_group.hh"
#include "saltus/shader_pack.hh"
#include "saltus/mesh.hh"
#include "saltus/sampler.hh"
#include "saltus/texture.hh"
#include "saltus/window.hh"
#include "saltus/shader.hh"
#include "saltus/buffer.hh"

namespace saltus
{
    enum class RendererPresentMode
    {
        Immediate,
        Mailbox,
        VSync,
    };
    std::ostream& operator <<(std::ostream& outs, RendererPresentMode present_mode);

    enum class MsaaSamples: uint32_t
    {
        Sample1 = 1,
        Sample2 = 2,
        Sample4 = 4,
        Sample8 = 8,
        Sample16 = 16,
        Sample32 = 32,
        Sample64 = 64,
    };

    struct RendererCreateInfo
    {
        Window &window;
        RendererPresentMode target_present_mode;
        MsaaSamples msaa_samples;
    };

    struct RenderInfo
    {
        const std::vector<std::shared_ptr<InstanceGroup>> &instance_groups;
        std::optional<matrix::Vector4F> clear_color;
    };

    class Renderer
    {
    public:
        virtual ~Renderer();

        static std::unique_ptr<Renderer> create(RendererCreateInfo);

        Window &window();

        const RendererPresentMode &target_present_mode() const;
        virtual RendererPresentMode current_present_mode() const = 0;
        virtual void target_present_mode(RendererPresentMode);

        virtual matrix::Vector2<uint32_t> framebuffer_size() const = 0;

        virtual void render(RenderInfo info) = 0;
        virtual void wait_for_idle() = 0;

        virtual std::shared_ptr<Buffer> create_buffer(BufferCreateInfo) = 0;
        virtual std::shared_ptr<Image> create_image(ImageCreateInfo) = 0;
        virtual std::shared_ptr<Sampler> create_sampler(SamplerCreateInfo) = 0;
        virtual std::shared_ptr<Texture> create_texture(TextureCreateInfo) = 0;
        virtual std::shared_ptr<Shader> create_shader(ShaderCreateInfo) = 0;
        virtual std::shared_ptr<ShaderPack> create_shader_pack(ShaderPackCreateInfo) = 0;
        virtual std::shared_ptr<Mesh> create_mesh(MeshCreateInfo) = 0;
        virtual std::shared_ptr<BindGroupLayout> create_bind_group_layout(BindGroupLayoutCreateInfo) = 0;
        virtual std::shared_ptr<BindGroup> create_bind_group(BindGroupCreateInfo) = 0;
        virtual std::shared_ptr<InstanceGroup> create_instance_group(InstanceGroupCreateInfo) = 0;

    protected:
        Renderer(RendererCreateInfo);

        Window &window_;
        RendererPresentMode target_present_mode_;
    };
} // namespace saltus

