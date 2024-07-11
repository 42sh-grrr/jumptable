#pragma once

#include <string>
#include <memory>

#include "saltus/bind_group_layout.hh"
#include "saltus/shader.hh"
#include "saltus/vertex_attribute.hh"

namespace saltus
{
    enum class ShaderPackCullMode: uint8_t
    {
        /// Show both sides
        None,
        /// Hide the back
        Back,
        /// Hide the front
        Front,
        /// Hide all triangles
        All,
    };

    enum class ShaderPackFrontFace: uint8_t
    {
        CounterClockwise,
        Clockwise,
    };

    struct ShaderPackVertexAttribute
    {
        uint32_t location;
        std::string name;
        VertexAttributeType type;
    };

    struct ShaderPackCreateInfo
    {
        std::vector<std::shared_ptr<BindGroupLayout>> bind_group_layouts;
        std::vector<ShaderPackVertexAttribute> vertex_attributes;

        std::shared_ptr<Shader> vertex_shader;
        std::shared_ptr<Shader> fragment_shader;

        ShaderPackFrontFace front_face;
        ShaderPackCullMode cull_mode;
        PritmitiveTopology primitive_topology;

        ShaderPackCreateInfo();
    };

    class ShaderPack
    {
    public:
        virtual ~ShaderPack() = 0;
        ShaderPack(const ShaderPack &x) = delete;
        const ShaderPack &operator =(const ShaderPack &x) = delete;

        const std::vector<std::shared_ptr<BindGroupLayout>> &bind_group_layouts() const;
        const std::vector<ShaderPackVertexAttribute> &vertex_attributes() const;

        const std::shared_ptr<Shader> &vertex_shader() const;
        const std::shared_ptr<Shader> &fragment_shader() const;

        ShaderPackFrontFace front_face() const;
        ShaderPackCullMode cull_mode() const;
        PritmitiveTopology primitive_topology() const;
        
    protected:
        ShaderPack(ShaderPackCreateInfo create_info);

    private:
        std::vector<std::shared_ptr<BindGroupLayout>> bind_group_layouts_;
        std::vector<ShaderPackVertexAttribute> vertex_attributes_;

        std::shared_ptr<Shader> vertex_shader_;
        std::shared_ptr<Shader> fragment_shader_;

        ShaderPackFrontFace front_face_;
        ShaderPackCullMode cull_mode_;
        PritmitiveTopology primitive_topology_;
    };
} // namespace saltus

