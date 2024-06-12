#include "saltus/mesh.hh"

namespace saltus
{
    Mesh::Mesh(MeshCreateInfo info):
        vertex_attributes_(info.vertex_attributes),
        flip_faces_(info.flip_faces),
        primitive_topology_(info.primitive_topology)
    { }

    Mesh::~Mesh()
    { }

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
} // namespace saltus
