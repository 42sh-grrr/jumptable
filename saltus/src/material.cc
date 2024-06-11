#include "saltus/material.hh"
#include <stdexcept>

namespace saltus
{
    MaterialCreateInfo::MaterialCreateInfo()
    {
        // zero value is already a good default
    }

    Material::Material(MaterialCreateInfo create_info):
        vertex_shader_(create_info.vertex_shader),
        fragment_shader_(create_info.fragment_shader),
        vertex_attributes_(create_info.vertex_attributes)
    {
        if (!create_info.vertex_shader)
            throw std::runtime_error("Vertex shader of material cannot be null");
        if (!create_info.fragment_shader)
            throw std::runtime_error("Vertex shader of material cannot be null");
    }

    Material::~Material()
    { }

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
}
