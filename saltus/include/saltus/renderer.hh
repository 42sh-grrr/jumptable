#pragma once

#include <memory>

#include "matrix/vector.hh"
#include "saltus/bind_group.hh"
#include "saltus/instance_group.hh"
#include "saltus/material.hh"
#include "saltus/mesh.hh"
#include "saltus/window.hh"
#include "saltus/shader.hh"
#include "saltus/buffer.hh"

namespace saltus
{
    struct RenderInfo
    {
        const std::vector<std::shared_ptr<InstanceGroup>> &instance_groups;
        matrix::Vector4F clear_color;
    };

    class Renderer
    {
    public:
        virtual ~Renderer();

        static std::unique_ptr<Renderer> create(Window &window);

        Window &window();

        virtual void render(RenderInfo info) = 0;
        virtual void wait_for_idle() = 0;

        virtual std::shared_ptr<Buffer> create_buffer(BufferCreateInfo) = 0;
        virtual std::shared_ptr<Shader> create_shader(ShaderCreateInfo) = 0;
        virtual std::shared_ptr<Material> create_material(MaterialCreateInfo) = 0;
        virtual std::shared_ptr<Mesh> create_mesh(MeshCreateInfo) = 0;
        virtual std::shared_ptr<BindGroupLayout> create_bind_group_layout(BindGroupLayoutCreateInfo) = 0;
        virtual std::shared_ptr<BindGroup> create_bind_group(BindGroupCreateInfo) = 0;
        virtual std::shared_ptr<InstanceGroup> create_instance_group(InstanceGroupCreateInfo) = 0;

    protected:
        Renderer(Window &window);

        Window &window_;
    };
} // namespace saltus

