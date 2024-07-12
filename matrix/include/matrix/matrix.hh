#pragma once

#include <array>
#include <type_traits>

namespace matrix
{
    template <typename TYPE, int ROW>
    class Vector;

    template <typename TYPE, int ROW, int COL>
    class Matrix
    {
    private:
        static constexpr bool ENABLE = ROW > 0 && COL > 0;
        static constexpr bool ENABLE_TRACE = ROW == COL;
        static constexpr bool ENABLE_VECTOR_CONVERSION = COL == 1;

    public:
        using type = TYPE;
        static constexpr int rows = ROW;
        static constexpr int cols = COL;

        Matrix();
        Matrix(std::array<TYPE, ROW * COL> flat);

        Matrix<TYPE, ROW, COL>
        operator+(const Matrix<TYPE, ROW, COL> mat) const;
        Matrix<TYPE, ROW, COL>& operator+=(const Matrix<TYPE, ROW, COL> mat);

        Matrix<TYPE, ROW, COL>
        operator-(const Matrix<TYPE, ROW, COL> mat) const;
        Matrix<TYPE, ROW, COL>& operator-=(const Matrix<TYPE, ROW, COL> mat);

        template <int OUT_COL>
        Matrix<TYPE, ROW, OUT_COL>
        operator*(const Matrix<TYPE, COL, OUT_COL> mat) const;
        Matrix<TYPE, ROW, COL>& operator*=(const Matrix<TYPE, COL, COL> mat);

        Vector<TYPE, ROW> operator*(const Vector<TYPE, COL> vec) const;

        Matrix<TYPE, ROW, COL> operator*(TYPE scalar) const;
        Matrix<TYPE, ROW, COL>& operator*=(TYPE scalar);

        Matrix<TYPE, ROW, COL> operator/(TYPE scalar) const;
        Matrix<TYPE, ROW, COL>& operator/=(TYPE scalar);

        const TYPE* operator[](int idx) const;
        TYPE* operator[](int idx);

        template <bool E = ENABLE_TRACE,
                  typename std::enable_if<E, int>::type = 0>
        [[nodiscard]] TYPE trace() const;

        template <typename NORM_TYPE = TYPE>
        [[nodiscard]] NORM_TYPE norm() const;

        [[nodiscard]] Matrix<TYPE, COL, ROW> transpose() const;

        template <typename NORMALIZED_TYPE = TYPE>
        [[nodiscard]] Matrix<NORMALIZED_TYPE, ROW, COL> normalized() const;

        Matrix<TYPE, ROW, COL>& normalize();

        template <bool E = ENABLE_VECTOR_CONVERSION,
                  typename std::enable_if<E, int>::type = 0>
        operator Vector<TYPE, ROW>() const;

    protected:
        typename std::enable_if<ENABLE, TYPE>::type mat_[ROW][COL];
    };

    template <typename TYPE, int SIZE>
    Matrix<TYPE, SIZE, SIZE> identity();

    template <typename TYPE, int ROW, int COL, typename DOT_TYPE = TYPE>
    [[nodiscard]] DOT_TYPE dot(const Matrix<TYPE, ROW, COL>& mat,
                               const Matrix<TYPE, ROW, COL>& other);

    using Matrix2F = Matrix<float, 2, 2>;
    using Matrix3F = Matrix<float, 3, 3>;
    using Matrix4F = Matrix<float, 4, 4>;

    using Matrix2D = Matrix<double, 2, 2>;
    using Matrix3D = Matrix<double, 3, 3>;
    using Matrix4D = Matrix<double, 4, 4>;

    using Matrix2I = Matrix<int, 2, 2>;
    using Matrix3I = Matrix<int, 3, 3>;
    using Matrix4I = Matrix<int, 4, 4>;
} // namespace matrix

#include <matrix/matrix.hxx>
