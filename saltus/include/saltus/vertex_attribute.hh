#pragma once

#include <cstdint>

namespace saltus
{
    enum class PritmitiveTopology: uint8_t
    {
        TriangleList,
        TriangleStrip,
        LineList,
        LineStrip,
    };

    enum class VertexAttributeScalarType: uint8_t
    {
        u8,
        i8,
        u16,
        i16,
        u32,
        i32,
        f32,
        u64,
        i64,
        f64,
    };

    enum class VertexAttributeFormat: uint8_t
    {
        Scalar,
        Vec2,
        Vec3,
        Vec4,
    };

    struct VertexAttributeDataType
    {
        VertexAttributeScalarType scalar_type;
        VertexAttributeFormat format;
    };
}
