#include "saltus/vertex_attribute.hh"
#include <stdexcept>

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

    uint32_t VertexAttributeType::size() const
    {
        uint32_t data_type_size;
        switch (scalar_type)
        {
        case VertexAttributeDataType::u8:
        case VertexAttributeDataType::i8:
            data_type_size = 1;
            break;
        case VertexAttributeDataType::u16:
        case VertexAttributeDataType::i16:
            data_type_size = 2;
            break;
        case VertexAttributeDataType::u32:
        case VertexAttributeDataType::i32:
        case VertexAttributeDataType::f32:
            data_type_size = 4;
            break;
        case VertexAttributeDataType::u64:
        case VertexAttributeDataType::i64:
        case VertexAttributeDataType::f64:
            data_type_size = 8;
            break;
        }

        switch (format)
        {
        case VertexAttributeFormat::Scalar:
            return data_type_size;
        case VertexAttributeFormat::Vec2:
            return data_type_size*2;
        case VertexAttributeFormat::Vec3:
            return data_type_size*3;
        case VertexAttributeFormat::Vec4:
            return data_type_size*4;
        }

        throw std::runtime_error("Invalid format");
    }

    bool VertexAttributeType::operator ==(const VertexAttributeType &other) const
    {
        return scalar_type == other.scalar_type && format == other.format;
    }

    bool VertexAttributeType::operator !=(const VertexAttributeType &other) const
    {
        return !(*this == other);
    }
}
