#pragma once

#include <string>
#include <memory>

#include "saltus/shader.hh"
#include "saltus/vertex_attribute.hh"

namespace saltus
{
    enum class MaterialCullMode: uint8_t
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

    enum class MaterialFrontFace: uint8_t
    {
        CounterClockwise,
        Clockwise,
    };

    struct MaterialVertexAttribute
    {
        uint32_t location;
        std::string name;
        VertexAttributeType type;
    };

    struct MaterialCreateInfo
    {
        std::vector<MaterialVertexAttribute> vertex_attributes;

        std::shared_ptr<Shader> vertex_shader;
        std::shared_ptr<Shader> fragment_shader;

        MaterialFrontFace front_face;
        MaterialCullMode cull_mode;
        PritmitiveTopology primitive_topology;

        MaterialCreateInfo();
    };

    class Material
    {
    public:
        virtual ~Material() = 0;
        Material(const Material &x) = delete;
        const Material &operator =(const Material &x) = delete;

        const std::vector<MaterialVertexAttribute> &vertex_attributes() const;

        const std::shared_ptr<Shader> &vertex_shader() const;
        const std::shared_ptr<Shader> &fragment_shader() const;

        MaterialFrontFace front_face() const;
        MaterialCullMode cull_mode() const;
        PritmitiveTopology primitive_topology() const;
        
    protected:
        Material(MaterialCreateInfo create_info);

    private:
        std::vector<MaterialVertexAttribute> vertex_attributes_;

        std::shared_ptr<Shader> vertex_shader_;
        std::shared_ptr<Shader> fragment_shader_;

        MaterialFrontFace front_face_;
        MaterialCullMode cull_mode_;
        PritmitiveTopology primitive_topology_;
    };
} // namespace saltus

