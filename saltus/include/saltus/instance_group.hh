#pragma once

#include <memory>
#include "saltus/mesh.hh"
#include "saltus/material.hh"

namespace saltus
{
    struct InstanceGroupCreateInfo {
        std::shared_ptr<Material> material;
        std::shared_ptr<Mesh> mesh;
    };

    class InstanceGroup
    {
    public:
        virtual ~InstanceGroup() = 0;
        InstanceGroup(const InstanceGroup &x) = delete;
        const InstanceGroup &operator =(const InstanceGroup &x) = delete;

        const std::shared_ptr<Material> &material() const;
        const std::shared_ptr<Mesh> &mesh() const;
        
    protected:
        InstanceGroup(InstanceGroupCreateInfo info);

    private:
        std::shared_ptr<Material> material_;
        std::shared_ptr<Mesh> mesh_;
    };
} // namespace saltus
