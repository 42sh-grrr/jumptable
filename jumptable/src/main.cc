#include <iostream>
#include <matrix/matrix.hh>
#include <graphics/graphics.hh>
#include <graphics/window.hh>
#include <unistd.h>

int main()
{
    matrix::Matrix<float, 4, 4> mat;
    mat[0][0] = 42;
    std::cout << mat[0][0] << '\n';

    graphics::Window window = graphics::WindowBuilder()
        .title("bite")
        .build();
    window.run();

    sleep(1);

    return 0;
}
