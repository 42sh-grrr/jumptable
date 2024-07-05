#include "saltus/material.hh"
#include <stdexcept>

namespace saltus
{
    MaterialCreateInfo::MaterialCreateInfo()
    {
        // zero value is already a good default
        // this constructor is only to prevent struct literal instantiation
    }

    Material::Material(MaterialCreateInfo create_info):
        bind_group_layouts_(create_info.bind_group_layouts),
        vertex_attributes_(create_info.vertex_attributes),
        vertex_shader_(create_info.vertex_shader),
        fragment_shader_(create_info.fragment_shader),
        front_face_(create_info.front_face),
        cull_mode_(create_info.cull_mode),
        primitive_topology_(create_info.primitive_topology)
    {
        if (!create_info.vertex_shader)
            throw std::runtime_error("Vertex shader of material cannot be null");
        if (!create_info.fragment_shader)
            throw std::runtime_error("Vertex shader of material cannot be null");
    }

    Material::~Material()
    { }

    const std::vector<std::shared_ptr<BindGroupLayout>> &Material::bind_group_layouts() const
    {
        return bind_group_layouts_;
    }

    const std::vector<MaterialVertexAttribute> &Material::vertex_attributes() const
    {
        return vertex_attributes_;
    }

    const std::shared_ptr<Shader> &Material::vertex_shader() const
    {
        return vertex_shader_;
    }

    const std::shared_ptr<Shader> &Material::fragment_shader() const
    {
        return fragment_shader_;
    }

    MaterialFrontFace Material::front_face() const
    {
        return front_face_;
    }

    MaterialCullMode Material::cull_mode() const
    {
        return cull_mode_;
    }

    PritmitiveTopology Material::primitive_topology() const
    {
        return primitive_topology_;
    }
}
