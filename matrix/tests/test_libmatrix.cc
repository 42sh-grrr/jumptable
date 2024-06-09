#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <matrix/matrix.hh>

TestSuite(libmatrix);

Test(libmatrix, create_matrix)
{
    matrix::Matrix<int, 4, 4> mat;
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            int expected = 0;
            int actual = mat[i][j];
            cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                         actual);
        }
    }
}

Test(libmatrix, set_matrix)
{
    matrix::Matrix<int, 4, 4> mat;
    mat[3][2] = 42;
    int expected = 42;
    int actual = mat[3][2];
    cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected, actual);
}

Test(libmatrix, add_matrices)
{
    matrix::Matrix<int, 4, 4> mat1;
    matrix::Matrix<int, 4, 4> mat2;
    for (int i = 0; i < mat1.rows; i++)
    {
        for (int j = 0; j < mat1.cols; j++)
        {
            mat1[i][j] = i + 4 * j + i * j + 2;
            mat2[i][j] = 2 * j + 3 * i + 2 * i * i;
        }
    }
    matrix::Matrix<int, 4, 4> mat = mat1 + mat2;
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            int expected = 4 * i + 6 * j + 2 * i * i + i * j + 2;
            int actual = mat[i][j];
            cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                         actual);
        }
    }
}

Test(libmatrix, multiply_matrices)
{
    matrix::Matrix<int, 4, 4> mat1;
    matrix::Vector<int, 4> vec1;
    for (int i = 0; i < mat1.rows; i++)
    {
        for (int j = 0; j < mat1.cols; j++)
        {
            mat1[i][j] = i + j;
        }
        vec1[i][0] = i;
    }
    matrix::Vector<int, 4> vec = mat1 * vec1;
    for (int i = 0; i < vec.rows; i++)
    {
        int expected = 6 * i + 14;
        int actual = vec[i][0];
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
    }
}
