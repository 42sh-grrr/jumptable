#pragma once

#include <string>
#include <memory>

#include "saltus/shader.hh"
#include "saltus/vertex_attribute.hh"

namespace saltus
{
    enum class MaterialCullMode
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

    enum class MaterialFrontFace
    {
        CounterClockwise,
        Clockwise,
    };

    struct MaterialVertexAttribute
    {
        uint32_t location;
        std::string name;
        VertexAttributeDataType data_type;
    };

    struct MaterialCreateInfo
    {
        std::vector<MaterialVertexAttribute> vertex_attributes;

        std::shared_ptr<Shader> vertex_shader;
        std::shared_ptr<Shader> fragment_shader;

        MaterialFrontFace front_face;
        MaterialCullMode cull_mode;

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

    protected:
        Material(MaterialCreateInfo create_info);

    private:
        std::vector<MaterialVertexAttribute> vertex_attributes_;

        std::shared_ptr<Shader> vertex_shader_;
        std::shared_ptr<Shader> fragment_shader_;
    };
} // namespace saltus

