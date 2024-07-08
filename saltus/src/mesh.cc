#include "saltus/mesh.hh"
#include <stdexcept>

namespace saltus
{
    MeshCreateInfo::MeshCreateInfo()
    { }

    Mesh::Mesh(MeshCreateInfo info):
        vertex_count_(info.vertex_count),
        vertex_attributes_(info.vertex_attributes),
        flip_faces_(info.flip_faces),
        primitive_topology_(info.primitive_topology),
        index_format_(info.index_format),
        index_buffer_(info.index_buffer)
    {
        for (const auto &attr : info.vertex_attributes)
        {
            if (!attr.buffer->usages().vertex)
                throw std::runtime_error("Meshes vertex buffers must have the vertex usage");
        }
    }

    Mesh::~Mesh()
    { }

    uint32_t Mesh::vertex_count() const
    {
        return vertex_count_;
    }

    const std::vector<MeshVertexAttribute> &Mesh::vertex_attributes() const
    {
        return vertex_attributes_;
    }

    bool Mesh::flip_faces() const
    {
        return flip_faces_;
    }

    PritmitiveTopology Mesh::primitive_topology() const
    {
        return primitive_topology_;
    }

    const MeshIndexFormat &Mesh::index_format() const
    {
        return index_format_;
    }

    const std::shared_ptr<Buffer> &Mesh::index_buffer() const
    {
        return index_buffer_;
    }
} // namespace saltus
