#pragma once

#include "saltus/byte_array.hh"

namespace saltus
{
    template<typename T>
    ByteArray to_bytearray(const std::vector<T> &vec)
    {
        auto bytes = reinterpret_cast<const T *>(vec.data());
        return ByteArray(bytes, bytes + sizeof(T) * vec.size());
    }

    template<typename T, size_t len>
    ByteArray to_bytearray(const std::array<T, len> &array)
    {
        auto bytes = reinterpret_cast<const T *>(array.data);
        return ByteArray(bytes, bytes + sizeof(T) * len);
    }
} // namespace saltus
