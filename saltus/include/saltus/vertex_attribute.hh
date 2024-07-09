#pragma once

#include <cstdint>

namespace saltus
{
    /// Use google to know what they mean x)
    enum class PritmitiveTopology: uint8_t
    {
        TriangleList,
        TriangleStrip,
        LineList,
        LineStrip,
    };

    enum class VertexAttributeDataType: uint8_t
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

    struct VertexAttributeType
    {
        VertexAttributeDataType scalar_type;
        VertexAttributeFormat format;

        static VertexAttributeType Float;
        static VertexAttributeType Vec2f;
        static VertexAttributeType Vec3f;
        static VertexAttributeType Vec4f;

        uint32_t size() const;

        bool operator ==(const VertexAttributeType &) const;
        bool operator !=(const VertexAttributeType &) const;
    };
}
