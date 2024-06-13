#include "saltus/instance_group.hh"
#include <stdexcept>
#include "saltus/mesh.hh"

namespace saltus
{
    InstanceGroup::InstanceGroup(InstanceGroupCreateInfo info):
        material_(info.material),
        mesh_(info.mesh)
    {
        if (material_->primitive_topology() != mesh_->primitive_topology())
            throw std::runtime_error("InstanceGroup: Material's and mesh's primitive topology don't match");

        // make sure all material's attributes are present in the mesh
        for (const auto &material_attr : material_->vertex_attributes())
        {
            const MeshVertexAttribute *found = nullptr;
            for (const auto &mesh_attr : mesh_->vertex_attributes())
            {
                if (material_attr.name == mesh_attr.name)
                {
                    found = &mesh_attr;
                    break;
                }
            }
            if (!found)
                throw std::runtime_error("InstanceGroup: A material vertex attribute is missing from the mesh");
            if (found->type != material_attr.type)
                throw std::runtime_error("InstanceGroup: A material vertex attribute has a different type than the mesh");
        }
    }

    InstanceGroup::~InstanceGroup()
    { }

    const std::shared_ptr<Material> &InstanceGroup::material() const
    {
        return material_;
    }

    const std::shared_ptr<Mesh> &InstanceGroup::mesh() const
    {
        return mesh_;
    }
} // namespace saltus

