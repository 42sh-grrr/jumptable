#include "saltus/bind_group.hh"

namespace saltus
{
    BindGroup::BindGroup(BindGroupCreateInfo info):
        layout_(info.layout)
    { }

    BindGroup::~BindGroup()
    { }

    const std::shared_ptr<BindGroupLayout> &BindGroup::layout() const
    {
        return layout_;
    }
} // namespace saltus
