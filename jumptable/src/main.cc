#include <iostream>
#include <matrix/matrix.hh>

int main()
{
    matrix::Matrix<float, 4, 4> mat;
    mat[0][0] = 42;
    std::cout << mat[0][0] << '\n';

    return 0;
}
