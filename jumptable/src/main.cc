#include <iostream>
#include <matrix/matrix.hh>
#include <graphics/graphics.hh>

int main()
{
    matrix::Matrix<float, 4, 4> mat;
    mat[0][0] = 42;
    std::cout << mat[0][0] << '\n';

    graphics::Graphics _graphics;

    return 0;
}
