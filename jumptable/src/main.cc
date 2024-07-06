#include <iostream>
#include <matrix/matrix.hh>
#include <logger/logger.hh>

int main()
{
    logger::trace() << "Hello, world!\n";
    logger::debug() << "Hello, world!\n";
    logger::info() << "Hello, world!\n";
    logger::warn() << "Hello, world!\n";
    logger::error() << "Hello, world!\n";
    logger::fatal() << "Hello, world!\n";

    matrix::Matrix<float, 4, 4> mat;
    mat[0][0] = 42;
    std::cout << mat[0][0] << '\n';

    return 0;
}
