#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "saltus/fwd.hh"

namespace saltus
{
    struct BindGroupCreateInfo
    {
        std::shared_ptr<BindGroupLayout> layout;
    };

    class BindGroup
    {
    public:
        virtual ~BindGroup() = 0;
        BindGroup(const BindGroup&) = default;
        BindGroup& operator=(const BindGroup&) = default;

        const std::shared_ptr<BindGroupLayout> &layout() const;

        virtual void set_binding(
            uint32_t binding_id,
            const std::shared_ptr<Buffer> &buffer,
            uint32_t array_index = 0,
            uint64_t offset = 0,
            std::optional<uint64_t> size = std::nullopt
        ) = 0;

        virtual void set_binding(
            uint32_t binding_id,
            const std::shared_ptr<Texture> &buffer,
            uint32_t array_index = 0
        ) = 0;

    protected:
        BindGroup(BindGroupCreateInfo);

    private:
        std::shared_ptr<BindGroupLayout> layout_;
    };
} // namespace saltus
