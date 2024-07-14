#pragma once

#include <memory>
#include <string>
#include <vector>

#include "saltus/fwd.hh"
#include "saltus/vertex_attribute.hh"

namespace saltus
{
    struct MeshVertexAttribute
    {
        std::string name;
        VertexAttributeType type;
        /// Must have the Vertex usage
        std::shared_ptr<Buffer> buffer;
    };

    enum class MeshIndexFormat
    {
        UInt16,
        UInt32,
    };

    struct MeshCreateInfo
    {
        uint32_t vertex_count;
        std::vector<MeshVertexAttribute> vertex_attributes;
        /// Flips which faces are considered front face from the shader pack
        bool flip_faces;
        PritmitiveTopology primitive_topology;

        /// Ignored if no index buffer is given
        MeshIndexFormat index_format;
        /// Can be null if not indexed
        std::shared_ptr<Buffer> index_buffer;

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

        const MeshIndexFormat &index_format() const;
        const std::shared_ptr<Buffer> &index_buffer() const;

    protected:
        Mesh(MeshCreateInfo);

    private:
        uint32_t vertex_count_;
        std::vector<MeshVertexAttribute> vertex_attributes_;
        bool flip_faces_;
        PritmitiveTopology primitive_topology_;

        MeshIndexFormat index_format_;
        std::shared_ptr<Buffer> index_buffer_;
    };
} // namespace saltus
