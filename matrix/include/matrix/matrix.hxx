#pragma once

#include <matrix/matrix.hh>

namespace matrix
{
    template <typename TYPE, int ROW, int COL>
    const TYPE* Matrix<TYPE, ROW, COL>::operator[](int idx) const
    {
        return mat_[idx];
    }

    template <typename TYPE, int ROW, int COL>
    TYPE* Matrix<TYPE, ROW, COL>::operator[](int idx)
    {
        return mat_[idx];
    }
} // namespace matrix
