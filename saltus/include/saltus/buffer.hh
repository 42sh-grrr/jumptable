#pragma once

#include <cstdint>
#include <optional>

#include "saltus/byte_array.hh"

namespace saltus
{
    struct BufferUsages
    {
        bool uniform: 1;
        bool index: 1;
        bool vertex: 1;

        BufferUsages with_uniform();
        BufferUsages with_index();
        BufferUsages with_vertex();
    };

    enum class BufferAccessHint
    {
        /// write << reads (i.e. world/mesh data)
        Static,
        /// write ~= reads (i.e. each frames)
        Dynamic,
    };

    struct BufferCreateInfo
    {
        BufferUsages usages;
        BufferAccessHint access_hint;
        /// Allocated buffer size
        /// Can be non 0 with null data in which case data is kept uninitialized
        std::size_t size;
        /// Can be null, if not must be an array of the given size
        const uint8_t *data;
    };

    BufferCreateInfo buffer_from_byte_array(
        BufferUsages usages,
        BufferAccessHint access_hint,
        const ByteArray &data
    );

    class Buffer
    {
    public:
        virtual ~Buffer() = 0;
        Buffer(const Buffer&) = default;
        Buffer& operator=(const Buffer&) = default;

        BufferUsages usages() const;
        BufferAccessHint access_hint() const;
        std::size_t size() const;

        virtual void write(
            const uint8_t *data,
            uint64_t offset = 0,
            std::optional<uint64_t> size = std::nullopt
        ) = 0;

    protected:
        Buffer(BufferCreateInfo info);

    private:
        BufferUsages usages_;
        BufferAccessHint access_hint_;
        std::size_t size_;
    };
} // namespace saltus
