#pragma once

#include <cstdint>
#include <vector>

namespace saltus
{
    using ByteArray = const std::vector<uint8_t>;

    template<typename T>
    ByteArray to_bytearray(const std::vector<T> &);
    template<typename T, size_t len>
    ByteArray to_bytearray(const std::array<T, len> &);
} // namespace saltus

#include <saltus/byte_array.hxx>
