#include <saltus/bind_group_layout.hh>

namespace saltus
{
    BindGroupLayout::BindGroupLayout(BindGroupLayoutCreateInfo info):
        bindings_(info.bindings)
    {
        
    }

    BindGroupLayout::~BindGroupLayout()
    { }

    const std::vector<BindGroupLayoutBindingInfo> &BindGroupLayout::bindings() const
    {
        return bindings_;
    }

    const BindGroupLayoutBindingInfo*
    BindGroupLayout::get_binding(uint32_t binding_id) const
    {
        for (const auto &binding : bindings_)
        {
            if (binding.binding_id == binding_id)
                return &binding;
        }
        return nullptr;
    }
} // namespace saltus
