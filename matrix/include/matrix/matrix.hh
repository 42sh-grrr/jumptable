#pragma once

#include <array>

namespace matrix
{
    template <typename TYPE, int ROW>
    class Vector;

    template <typename TYPE, int ROW, int COL>
    class Matrix
    {
    private:
        static constexpr bool ENABLE = ROW > 0 && COL > 0;

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

        const TYPE* operator[](int idx) const;
        TYPE* operator[](int idx);

        Matrix<TYPE, COL, ROW> transpose() const;

    protected:
        typename std::enable_if<ENABLE, TYPE>::type mat_[ROW][COL];
    };

    template <typename TYPE, int SIZE>
    Matrix<TYPE, SIZE, SIZE> identity();

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
