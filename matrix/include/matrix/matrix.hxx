#pragma once

#include <matrix/matrix.hh>

namespace matrix
{
    template <typename TYPE, int ROW, int COL>
    Matrix<TYPE, ROW, COL>::Matrix()
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                mat_[i][j] = 0;
            }
        }
    }

    template <typename TYPE, int ROW, int COL>
    Matrix<TYPE, ROW, COL>
    Matrix<TYPE, ROW, COL>::operator+(const Matrix<TYPE, ROW, COL> mat) const
    {
        Matrix<TYPE, ROW, COL> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = mat_[i][j] + mat[i][j];
            }
        }
        return res;
    }

    template <typename TYPE, int ROW, int COL>
    Matrix<TYPE, ROW, COL>
    Matrix<TYPE, ROW, COL>::operator-(const Matrix<TYPE, ROW, COL> mat) const
    {
        Matrix<TYPE, ROW, COL> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = mat_[i][j] - mat[i][j];
            }
        }
        return res;
    }

    template <typename TYPE, int ROW, int COL>
    template <int OUT_COL>
    Matrix<TYPE, ROW, OUT_COL> Matrix<TYPE, ROW, COL>::operator*(
        const Matrix<TYPE, COL, OUT_COL> mat) const
    {
        Matrix<TYPE, ROW, OUT_COL> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < OUT_COL; j++)
            {
                res[i][j] = 0;
                for (int k = 0; k < COL; k++)
                {
                    res[i][j] += mat_[i][k] * mat[k][j];
                }
            }
        }
        return res;
    }

    template <typename TYPE, int ROW, int COL>
    inline const TYPE* Matrix<TYPE, ROW, COL>::operator[](int idx) const
    {
        return mat_[idx];
    }

    template <typename TYPE, int ROW, int COL>
    inline TYPE* Matrix<TYPE, ROW, COL>::operator[](int idx)
    {
        return mat_[idx];
    }
} // namespace matrix
