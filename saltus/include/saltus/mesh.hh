#pragma once

#include <string>
#include <vector>

#include "saltus/byte_array.hh"
#include "saltus/vertex_attribute.hh"

namespace saltus
{
    struct MeshVertexAttribute
    {
        std::string name;
        VertexAttributeType type;
        ByteArray data;
    };

    struct MeshCreateInfo
    {
        uint32_t vertex_count;
        std::vector<MeshVertexAttribute> vertex_attributes;
        /// Flips which faces are conridered front face from the material
        bool flip_faces;
        PritmitiveTopology primitive_topology;

        MeshCreateInfo();
    };

    class Mesh
    {
    public:
        virtual ~Mesh() = 0;
        Mesh(const Mesh &x) = delete;
        const Mesh &operator =(const Mesh &x) = delete;

        uint32_t vertex_count() const;
        const std::vector<MeshVertexAttribute> &vertex_attributes() const;
        bool flip_faces() const;
        PritmitiveTopology primitive_topology() const;

    protected:
        Mesh(MeshCreateInfo);

    private:
        uint32_t vertex_count_;
        std::vector<MeshVertexAttribute> vertex_attributes_;
        bool flip_faces_;
        PritmitiveTopology primitive_topology_;
    };
} // namespace saltus
