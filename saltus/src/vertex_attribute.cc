#include "saltus/vertex_attribute.hh"

namespace saltus
{
    VertexAttributeType VertexAttributeType::Vec2f {
        .scalar_type = VertexAttributeDataType::f32,
        .format = VertexAttributeFormat::Vec2,
    };
    VertexAttributeType VertexAttributeType::Vec3f {
        .scalar_type = VertexAttributeDataType::f32,
        .format = VertexAttributeFormat::Vec3,
    };
    VertexAttributeType VertexAttributeType::Vec4f {
        .scalar_type = VertexAttributeDataType::f32,
        .format = VertexAttributeFormat::Vec4,
    };

    bool VertexAttributeType::operator ==(const VertexAttributeType &other) const
    {
        return scalar_type == other.scalar_type && format == other.format;
    }

    bool VertexAttributeType::operator !=(const VertexAttributeType &other) const
    {
        return !(*this == other);
    }
}
