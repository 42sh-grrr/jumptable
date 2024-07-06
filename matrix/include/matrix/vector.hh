#pragma once

#include <matrix/matrix.hh>

namespace matrix
{
    template <typename TYPE, int ROW>
    class Vector : public Matrix<TYPE, ROW, 1>
    {
    private:
        static constexpr bool ENABLE_X = ROW >= 1 && ROW <= 4;
        static constexpr bool ENABLE_Y = ROW >= 2 && ROW <= 4;
        static constexpr bool ENABLE_Z = ROW == 3 || ROW == 4;
        static constexpr bool ENABLE_W = ROW == 4;

    public:
        using type = TYPE;
        static constexpr int rows = ROW;

        using super = Matrix<TYPE, ROW, 1>;

        Vector();
        Vector(std::array<TYPE, ROW> flat);

        const TYPE& operator[](int idx) const;
        TYPE& operator[](int idx);

        template <bool E = ENABLE_X, typename std::enable_if<E, int>::type = 0>
        inline TYPE& x()
        {
            return super::mat_[0][0];
        }
        template <bool E = ENABLE_X, typename std::enable_if<E, int>::type = 0>
        inline const TYPE& x() const
        {
            return super::mat_[0][0];
        }

        template <bool E = ENABLE_Y, typename std::enable_if<E, int>::type = 0>
        inline TYPE& y()
        {
            return super::mat_[1][0];
        }
        template <bool E = ENABLE_Y, typename std::enable_if<E, int>::type = 0>
        inline const TYPE& y() const
        {
            return super::mat_[1][0];
        }

        template <bool E = ENABLE_Z, typename std::enable_if<E, int>::type = 0>
        inline TYPE& z()
        {
            return super::mat_[2][0];
        }
        template <bool E = ENABLE_Z, typename std::enable_if<E, int>::type = 0>
        inline const TYPE& z() const
        {
            return super::mat_[2][0];
        }

        template <bool E = ENABLE_W, typename std::enable_if<E, int>::type = 0>
        inline TYPE& w()
        {
            return super::mat_[3][0];
        }
        template <bool E = ENABLE_W, typename std::enable_if<E, int>::type = 0>
        inline const TYPE& w() const
        {
            return super::mat_[3][0];
        }
    };

    using Vector2F = Vector<float, 2>;
    using Vector3F = Vector<float, 3>;
    using Vector4F = Vector<float, 4>;

    using Vector2D = Vector<double, 2>;
    using Vector3D = Vector<double, 3>;
    using Vector4D = Vector<double, 4>;

    using Vector2I = Vector<int, 2>;
    using Vector3I = Vector<int, 3>;
    using Vector4I = Vector<int, 4>;
} // namespace matrix

#include <matrix/vector.hxx>
