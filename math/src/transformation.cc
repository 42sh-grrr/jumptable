#include <cmath>
#include <math/transformation.hh>
#include <matrix/matrix.hh>

namespace math::transformation
{
    /// Float implementation
    matrix::Matrix3F scale2D(float dx, float dy)
    {
        matrix::Matrix3F mat = matrix::identity<float, 3>();
        mat[0][0] = dx;
        mat[1][1] = dy;
        return mat;
    }

    matrix::Matrix3F scale2D(float ds)
    {
        return scale2D(ds, ds);
    }

    matrix::Matrix4F scale3D(float dx, float dy, float dz)
    {
        matrix::Matrix4F mat = matrix::identity<float, 4>();
        mat[0][0] = dx;
        mat[1][1] = dy;
        mat[2][2] = dz;
        return mat;
    }

    matrix::Matrix4F scale3D(float ds)
    {
        return scale3D(ds, ds, ds);
    }

    matrix::Matrix3F translate2D(float dx, float dy)
    {
        matrix::Matrix3F mat = matrix::identity<float, 3>();
        mat[0][2] = dx;
        mat[1][2] = dy;
        return mat;
    }

    matrix::Matrix3F translate2D(matrix::Vector2F du)
    {
        return translate2D(du.x(), du.y());
    }

    matrix::Matrix4F translate3D(float dx, float dy, float dz)
    {
        matrix::Matrix4F mat = matrix::identity<float, 4>();
        mat[0][3] = dx;
        mat[1][3] = dy;
        mat[2][3] = dz;
        return mat;
    }

    matrix::Matrix4F translate3D(matrix::Vector3F du)
    {
        return translate3D(du.x(), du.y(), du.z());
    }

    matrix::Matrix3F rotate2D(float theta)
    {
        matrix::Matrix3F mat = matrix::identity<float, 3>();
        mat[0][0] = std::cos(theta);
        mat[0][1] = -std::sin(theta);
        mat[1][0] = std::sin(theta);
        mat[1][1] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4F rotate3Dx(float theta)
    {
        matrix::Matrix4F mat = matrix::identity<float, 4>();
        mat[1][1] = std::cos(theta);
        mat[1][2] = -std::sin(theta);
        mat[2][1] = std::sin(theta);
        mat[2][2] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4F rotate3Dy(float theta)
    {
        matrix::Matrix4F mat = matrix::identity<float, 4>();
        mat[2][2] = std::cos(theta);
        mat[2][0] = -std::sin(theta);
        mat[0][2] = std::sin(theta);
        mat[0][0] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4F rotate3Dz(float theta)
    {
        matrix::Matrix4F mat = matrix::identity<float, 4>();
        mat[0][0] = std::cos(theta);
        mat[0][1] = -std::sin(theta);
        mat[1][0] = std::sin(theta);
        mat[1][1] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4F perspective(float ar, float z_near, float z_far,
                                 float alpha)
    {
        float tan_half_alpha = std::tan(alpha / 2.0f);
        float z_range = z_far - z_near;

        matrix::Matrix4F mat;
        mat[0][0] = 1 / (ar * tan_half_alpha);
        mat[1][1] = 1 / tan_half_alpha;
        mat[2][2] = (z_far + z_near) / z_range;
        mat[2][3] = -2.0f * z_near * z_far / z_range;
        mat[3][2] = 1.0f;
        return mat;
    }

    matrix::Matrix4F orthographic(float left, float right, float top,
                                  float bottom, float z_near, float z_far)
    {
        float x_range = right - left;
        float y_range = top - bottom;
        float z_range = z_far - z_near;

        matrix::Matrix4F mat;
        mat[0][0] = 2.0f / x_range;
        mat[1][1] = 2.0f / y_range;
        mat[2][2] = -2.0f / z_range;
        mat[0][3] = -(right + left) / x_range;
        mat[1][3] = -(top + bottom) / y_range;
        mat[2][3] = -(z_far + z_near) / z_range;
        return mat;
    }

    /// Double implementation
    matrix::Matrix3D scale2D(double dx, double dy)
    {
        matrix::Matrix3D mat = matrix::identity<double, 3>();
        mat[0][0] = dx;
        mat[1][1] = dy;
        return mat;
    }

    matrix::Matrix3D scale2D(double ds)
    {
        return scale2D(ds, ds);
    }

    matrix::Matrix4D scale3D(double dx, double dy, double dz)
    {
        matrix::Matrix4D mat = matrix::identity<double, 4>();
        mat[0][0] = dx;
        mat[1][1] = dy;
        mat[2][2] = dz;
        return mat;
    }

    matrix::Matrix4D scale3D(double ds)
    {
        return scale3D(ds, ds, ds);
    }

    matrix::Matrix3D translate2D(double dx, double dy)
    {
        matrix::Matrix3D mat = matrix::identity<double, 3>();
        mat[0][2] = dx;
        mat[1][2] = dy;
        return mat;
    }

    matrix::Matrix3D translate2D(matrix::Vector2D du)
    {
        return translate2D(du.x(), du.y());
    }

    matrix::Matrix4D translate3D(double dx, double dy, double dz)
    {
        matrix::Matrix4D mat = matrix::identity<double, 4>();
        mat[0][3] = dx;
        mat[1][3] = dy;
        mat[2][3] = dz;
        return mat;
    }

    matrix::Matrix4D translate3D(matrix::Vector3D du)
    {
        return translate3D(du.x(), du.y(), du.z());
    }

    matrix::Matrix3D rotate2D(double theta)
    {
        matrix::Matrix3D mat = matrix::identity<double, 3>();
        mat[0][0] = std::cos(theta);
        mat[0][1] = -std::sin(theta);
        mat[1][0] = std::sin(theta);
        mat[1][1] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4D rotate3Dx(double theta)
    {
        matrix::Matrix4D mat = matrix::identity<double, 4>();
        mat[1][1] = std::cos(theta);
        mat[1][2] = -std::sin(theta);
        mat[2][1] = std::sin(theta);
        mat[2][2] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4D rotate3Dy(double theta)
    {
        matrix::Matrix4D mat = matrix::identity<double, 4>();
        mat[2][2] = std::cos(theta);
        mat[2][0] = -std::sin(theta);
        mat[0][2] = std::sin(theta);
        mat[0][0] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4D rotate3Dz(double theta)
    {
        matrix::Matrix4D mat = matrix::identity<double, 4>();
        mat[0][0] = std::cos(theta);
        mat[0][1] = -std::sin(theta);
        mat[1][0] = std::sin(theta);
        mat[1][1] = std::cos(theta);
        return mat;
    }

    matrix::Matrix4D perspective(double ar, double z_near,
                                 double z_far, double alpha)
    {
        double tan_half_alpha = std::tan(alpha / 2.0);
        double z_range = z_far - z_near;

        matrix::Matrix4D mat;
        mat[0][0] = 1 / (ar * tan_half_alpha);
        mat[1][1] = 1 / tan_half_alpha;
        mat[2][2] = (z_far + z_near) / z_range;
        mat[2][3] = -2.0 * z_near * z_far / z_range;
        mat[3][2] = 1.0;
        return mat;
    }

    matrix::Matrix4D orthographic(double left, double right, double top,
                                  double bottom, double z_near, double z_far)
    {
        double x_range = right - left;
        double y_range = top - bottom;
        double z_range = z_far - z_near;

        matrix::Matrix4D mat;
        mat[0][0] = 2.0 / x_range;
        mat[1][1] = 2.0 / y_range;
        mat[2][2] = -2.0 / z_range;
        mat[0][3] = -(right + left) / x_range;
        mat[1][3] = -(top + bottom) / y_range;
        mat[2][3] = -(z_far + z_near) / z_range;
        return mat;
    }
}; // namespace math::transformation
