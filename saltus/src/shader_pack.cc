#include "saltus/shader_pack.hh"
#include <stdexcept>

namespace saltus
{
    ShaderPackCreateInfo::ShaderPackCreateInfo()
    {
        // zero value is already a good default
        // this constructor is only to prevent struct literal instantiation
    }

    ShaderPack::ShaderPack(ShaderPackCreateInfo create_info):
        bind_group_layouts_(create_info.bind_group_layouts),
        vertex_attributes_(create_info.vertex_attributes),
        vertex_shader_(create_info.vertex_shader),
        fragment_shader_(create_info.fragment_shader),
        front_face_(create_info.front_face),
        cull_mode_(create_info.cull_mode),
        primitive_topology_(create_info.primitive_topology)
    {
        if (!create_info.vertex_shader)
            throw std::runtime_error("Vertex shader of shader pack cannot be null");
        if (!create_info.fragment_shader)
            throw std::runtime_error("Vertex shader of shader pack cannot be null");
    }

    ShaderPack::~ShaderPack()
    { }

    const std::vector<std::shared_ptr<BindGroupLayout>> &ShaderPack::bind_group_layouts() const
    {
        return bind_group_layouts_;
    }

    const std::vector<ShaderPackVertexAttribute> &ShaderPack::vertex_attributes() const
    {
        return vertex_attributes_;
    }

    const std::shared_ptr<Shader> &ShaderPack::vertex_shader() const
    {
        return vertex_shader_;
    }

    const std::shared_ptr<Shader> &ShaderPack::fragment_shader() const
    {
        return fragment_shader_;
    }

    ShaderPackFrontFace ShaderPack::front_face() const
    {
        return front_face_;
    }

    ShaderPackCullMode ShaderPack::cull_mode() const
    {
        return cull_mode_;
    }

    PritmitiveTopology ShaderPack::primitive_topology() const
    {
        return primitive_topology_;
    }
}
