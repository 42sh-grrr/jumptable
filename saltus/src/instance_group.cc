#include "saltus/instance_group.hh"
#include <stdexcept>
#include "saltus/mesh.hh"

namespace saltus
{
    InstanceGroup::InstanceGroup(InstanceGroupCreateInfo info):
        shader_pack__(info.shader_pack),
        mesh_(info.mesh),
        bind_groups_(info.bind_groups)
    {
        if (shader_pack__->primitive_topology() != mesh_->primitive_topology())
            throw std::runtime_error("InstanceGroup: shader_pack's and mesh's primitive topology don't match");

        // make sure all shader_pack's attributes are present in the mesh
        for (const auto &shader_pack_attr : shader_pack__->vertex_attributes())
        {
            const MeshVertexAttribute *found = nullptr;
            for (const auto &mesh_attr : mesh_->vertex_attributes())
            {
                if (shader_pack_attr.name == mesh_attr.name)
                {
                    found = &mesh_attr;
                    break;
                }
            }
            if (!found)
                throw std::runtime_error("InstanceGroup: A shader_pack vertex attribute is missing from the mesh");
            if (found->type != shader_pack_attr.type)
                throw std::runtime_error("InstanceGroup: A shader_pack vertex attribute has a different type than the mesh");
        }
    }

    InstanceGroup::~InstanceGroup()
    { }

    const std::shared_ptr<ShaderPack> &InstanceGroup::shader_pack() const
    {
        return shader_pack__;
    }

    const std::shared_ptr<Mesh> &InstanceGroup::mesh() const
    {
        return mesh_;
    }

    const std::vector<std::shared_ptr<BindGroup>> &InstanceGroup::bind_groups() const
    {
        return bind_groups_;
    }
} // namespace saltus

