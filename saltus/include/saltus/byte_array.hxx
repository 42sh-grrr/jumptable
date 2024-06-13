#pragma once

#include "saltus/byte_array.hh"

namespace saltus
{
    template<typename T>
    ByteArray to_bytearray(const std::vector<T> &vec)
    {
        auto bytes = reinterpret_cast<const uint8_t *>(vec.data());
        return ByteArray(bytes, bytes + sizeof(T) * vec.size());
    }

    template<typename T, size_t len>
    ByteArray to_bytearray(const std::array<T, len> &array)
    {
        auto bytes = reinterpret_cast<const uint8_t *>(array.data);
        return ByteArray(bytes, bytes + sizeof(T) * len);
    }
} // namespace saltus
