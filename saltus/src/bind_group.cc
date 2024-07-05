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

    void BindGroup::set_binding(
        uint32_t binding_id,
        const std::shared_ptr<Buffer> &buffer
    ) {
        set_binding(binding_id, 0, std::span(&buffer, 1));
    }
    void BindGroup::set_binding(
        uint32_t binding_id,
        uint32_t array_index,
        const std::shared_ptr<Buffer> &buffer
    ) {
        set_binding(binding_id, array_index, std::span(&buffer, 1));
    }
} // namespace saltus
