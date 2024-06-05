#pragma once

#include <ostream>

namespace matrix
{
    template <typename TYPE, int ROW, int COL>
    class Matrix
    {
    public:
        using type = TYPE;
        static constexpr int rows = ROW;
        static constexpr int cols = COL;

        const type* operator[](int idx) const;
        type* operator[](int idx);

        // friend std::ostream &operator<<(std::ostream &os,
        //                                 const Matrix<TYPE, ROW, COL>);

    private:
        type mat_[rows][cols];
    };

    using Matrix4F = Matrix<float, 4, 4>;
} // namespace matrix

#include <matrix/matrix.hxx>
