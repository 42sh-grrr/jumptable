#include <criterion/criterion.h>
#include <matrix/matrix.hh>
#include <matrix/vector.hh>

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

Test(libmatrix, create_matrix_flat)
{
    matrix::Matrix<int, 4, 4> mat(
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            int expected = (i == j) ? 1 : 0;
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
        vec1[i] = i;
    }
    matrix::Vector<int, 4> vec = mat1 * vec1;
    for (int i = 0; i < vec.rows; i++)
    {
        int expected = 6 * i + 14;
        int actual = vec[i];
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
    }
}

Test(libmatrix, create_vector_flat)
{
    matrix::Vector<int, 4> vec(
        { 0, 0, 0, 1 });
    for (int i = 0; i < vec.rows; i++)
    {
        int expected = (i == vec.rows - 1) ? 1 : 0;
        int actual = vec[i];
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
    }
}

Test(libmatrix, vector_accessors)
{
    matrix::Vector<int, 3> vec;
    vec.x() = 5;
    vec.y() = 6;
    vec.z() = 7;
    for (int i = 0; i < vec.rows; i++)
    {
        int expected = i + 5;
        int actual = vec[i];
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
        vec[i] = 2 * i;
    }
    {
        int expected = 0;
        int actual = vec.x();
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
    }
    {
        int expected = 2;
        int actual = vec.y();
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
    }
    {
        int expected = 4;
        int actual = vec.z();
        cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                     actual);
    }
}

Test(libmatrix, multiply_scalar)
{
    matrix::Matrix<int, 4, 4> mat1;
    for (int i = 0; i < mat1.rows; i++)
    {
        for (int j = 0; j < mat1.cols; j++)
        {
            mat1[i][j] = 4 * i + j;
        }
    }
    matrix::Matrix<int, 4, 4> mat = mat1 * 5;
    for (int i = 0; i < mat1.rows; i++)
    {
        for (int j = 0; j < mat1.cols; j++)
        {
            int expected = 20 * i + 5 * j;
            int actual = mat[i][j];
            cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                         actual);
        }
    }
}

Test(libmatrix, identity)
{
    matrix::Matrix<int, 4, 4> mat = matrix::identity<int, 4>();
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            int expected = (i == j) ? 1 : 0;
            int actual = mat[i][j];
            cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                         actual);
        }
    }
}

Test(libmatrix, plus_assign)
{
    matrix::Matrix<int, 4, 4> mat;
    matrix::Matrix<int, 4, 4> mat1;
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            mat[i][j] = i + 4 * j + i * j + 2;
            mat1[i][j] = 2 * j + 3 * i + 2 * i * i;
        }
    }
    mat += mat1;
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

Test(libmatrix, multiply_assign)
{
    matrix::Matrix<int, 2, 4> mat;
    matrix::Matrix<int, 4, 4> mat1;
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            mat[i][j] = 4 * i + j;
        }
    }
    for (int j = 0; j < mat1.cols; j++)
    {
        mat1[j][j] = j + 1;
    }
    mat *= mat1;
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            int expected = 4 * i + j + 4 * i * j + j * j;
            int actual = mat[i][j];
            cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                         actual);
        }
    }
}

Test(libmatrix, transpose_matrix)
{
    matrix::Matrix<int, 2, 4> mat1;
    for (int i = 0; i < mat1.rows; i++)
    {
        for (int j = 0; j < mat1.cols; j++)
        {
            mat1[i][j] = i * j + 2 * i;
        }
    }
    matrix::Matrix<int, 4, 2> mat = mat1.transpose();
    for (int i = 0; i < mat.rows; i++)
    {
        for (int j = 0; j < mat.cols; j++)
        {
            int expected = mat1[j][i];
            int actual = mat[i][j];
            cr_expect_eq(actual, expected, "Expected %d. Got %d.", expected,
                         actual);
        }
    }
}
