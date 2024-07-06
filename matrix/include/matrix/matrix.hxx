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
    Matrix<TYPE, ROW, COL>::Matrix(std::array<TYPE, ROW * COL> flat)
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                mat_[i][j] = flat[i * COL + j];
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
    Matrix<TYPE, ROW, COL>&
    Matrix<TYPE, ROW, COL>::operator+=(const Matrix<TYPE, ROW, COL> mat)
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                mat_[i][j] += mat[i][j];
            }
        }
        return *this;
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
    Matrix<TYPE, ROW, COL>&
    Matrix<TYPE, ROW, COL>::operator-=(const Matrix<TYPE, ROW, COL> mat)
    {
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                mat_[i][j] -= mat[i][j];
            }
        }
        return *this;
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
    Matrix<TYPE, ROW, COL>&
    Matrix<TYPE, ROW, COL>::operator*=(const Matrix<TYPE, COL, COL> mat)
    {
        TYPE row[COL];
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                row[j] = 0;
                for (int k = 0; k < COL; k++)
                {
                    row[j] += mat_[i][k] * mat[k][j];
                }
            }
            for (int j = 0; j < COL; j++)
            {
                mat_[i][j] = row[j];
            }
        }
        return *this;
    }

    template <typename TYPE, int ROW, int COL>
    Vector<TYPE, ROW>
    Matrix<TYPE, ROW, COL>::operator*(const Vector<TYPE, COL> vec) const
    {
        Vector<TYPE, ROW> res;
        for (int i = 0; i < ROW; i++)
        {
            res[i] = 0;
            for (int k = 0; k < COL; k++)
            {
                res[i] += mat_[i][k] * vec[k];
            }
        }
        return res;
    }

    template <typename TYPE, int ROW, int COL>
    Matrix<TYPE, ROW, COL> Matrix<TYPE, ROW, COL>::operator*(TYPE scalar) const
    {
        Matrix<TYPE, ROW, COL> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[i][j] = mat_[i][j] * scalar;
            }
        }
        return res;
    }

    template <typename TYPE, int ROW, int COL>
    Matrix<TYPE, ROW, COL>& Matrix<TYPE, ROW, COL>::operator*=(TYPE scalar)
    {
        Matrix<TYPE, ROW, COL> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                mat_[i][j] *= scalar;
            }
        }
        return *this;
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

    template <typename TYPE, int ROW, int COL>
    Matrix<TYPE, COL, ROW> Matrix<TYPE, ROW, COL>::transpose() const
    {
        Matrix<TYPE, COL, ROW> res;
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                res[j][i] = mat_[i][j];
            }
        }
        return res;
    }

    template <typename TYPE, int SIZE>
    Matrix<TYPE, SIZE, SIZE> identity()
    {
        Matrix<TYPE, SIZE, SIZE> res;
        for (int i = 0; i < res.rows; i++)
        {
            res[i][i] = 1;
        }
        return res;
    }
} // namespace matrix
