#include <cmath>
#include <criterion/criterion.h>
#include <math/transformation.hh>
#include <matrix/matrix.hh>
#include <numbers>

#include "matrix/vector.hh"

#define EQ(x, y) (std::abs(x - y) <= 1.0e-5f)

TestSuite(libmath);

Test(libmath, rotate_2D)
{
    matrix::Matrix3D rot = math::transformation::rotate2D(std::numbers::pi / 4);
    double ref = std::numbers::sqrt2 / 2;
    {
        double actual = rot[0][0];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[0][1];
        cr_expect(EQ(actual, -ref), "Expected %.5f. Got %.5f.", -ref, actual);
    }
    {
        double actual = rot[1][0];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[1][1];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
}

Test(libmath, rotate_3Dx)
{
    matrix::Matrix4D rot =
        math::transformation::rotate3Dx(std::numbers::pi / 4);
    double ref = std::numbers::sqrt2 / 2;
    {
        double actual = rot[1][1];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[1][2];
        cr_expect(EQ(actual, -ref), "Expected %.5f. Got %.5f.", -ref, actual);
    }
    {
        double actual = rot[2][1];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[2][2];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
}

Test(libmath, rotate_3Dy)
{
    matrix::Matrix4D rot =
        math::transformation::rotate3Dy(std::numbers::pi / 4);
    double ref = std::numbers::sqrt2 / 2;
    {
        double actual = rot[2][2];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[2][0];
        cr_expect(EQ(actual, -ref), "Expected %.5f. Got %.5f.", -ref, actual);
    }
    {
        double actual = rot[0][2];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[0][0];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
}

Test(libmath, rotate_3Dz)
{
    matrix::Matrix4D rot =
        math::transformation::rotate3Dz(std::numbers::pi / 4);
    double ref = std::numbers::sqrt2 / 2;
    {
        double actual = rot[0][0];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[0][1];
        cr_expect(EQ(actual, -ref), "Expected %.5f. Got %.5f.", -ref, actual);
    }
    {
        double actual = rot[1][0];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
    {
        double actual = rot[1][1];
        cr_expect(EQ(actual, ref), "Expected %.5f. Got %.5f.", ref, actual);
    }
}

Test(libmath, transformation)
{
    matrix::Vector4F vec({ 1.0f, 0.0f, 0.0f, 1.0f });
    matrix::Matrix4F tr = math::transformation::translate3D(2.0f, -1.0f, -1.0f);
    matrix::Matrix4F rot = math::transformation::rotate3Dy(
        static_cast<float>(std::numbers::pi / 2));
    matrix::Vector4F actual = tr * rot * vec;
    matrix::Vector4F expected({ 2.0f, -1.0f, -2.0f, 1.0f });
    for (int i = 0; i < vec.rows; i++)
    {
        cr_expect(EQ(actual[i], expected[i]), "Expected %.5f. Got %.5f.",
                  expected[i], actual[i]);
    }
}
