
namespace saltus
{
    enum class VertexAttributeScalarType
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

    enum class VertexAttributeFormat
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
