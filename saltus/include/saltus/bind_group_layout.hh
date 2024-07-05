#pragma once

#include <cstdint>
#include <vector>

namespace saltus
{
    enum class BindingType
    {
        UniformBuffer,
        StorageBuffer,
    };

    struct BindGroupLayoutBindingInfo
    {
        BindingType type;
        uint32_t count = 1;
        uint32_t binding_id;
    };

    struct BindGroupLayoutCreateInfo
    {
        std::vector<BindGroupLayoutBindingInfo> bindings;
    };

    class BindGroupLayout
    {
    public:
        virtual ~BindGroupLayout() = 0;
        BindGroupLayout(const BindGroupLayout&) = default;
        BindGroupLayout& operator=(const BindGroupLayout&) = default;

        const std::vector<BindGroupLayoutBindingInfo> &bindings() const;

        const BindGroupLayoutBindingInfo *get_binding(uint32_t binding_id) const;

    protected:
        BindGroupLayout(BindGroupLayoutCreateInfo);

    private:
        std::vector<BindGroupLayoutBindingInfo> bindings_;
    };
} // namespace saltus

