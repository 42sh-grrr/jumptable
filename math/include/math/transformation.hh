#pragma once

#include <matrix/matrix.hh>
#include <matrix/vector.hh>

namespace math::transformation
{
    /// Float version
    matrix::Matrix3F scale2D(float dx, float dy);
    matrix::Matrix3F scale2D(float ds);
    matrix::Matrix4F scale3D(float dx, float dy, float dz);
    matrix::Matrix4F scale3D(float ds);

    matrix::Matrix3F translate2D(float dx, float dy);
    matrix::Matrix3F translate2D(matrix::Vector2F du);
    matrix::Matrix4F translate3D(float dx, float dy, float dz);
    matrix::Matrix4F translate3D(matrix::Vector3F du);

    matrix::Matrix3F rotate2D(float theta);
    matrix::Matrix4F rotate3Dx(float theta);
    matrix::Matrix4F rotate3Dy(float theta);
    matrix::Matrix4F rotate3Dz(float theta);

    matrix::Matrix4F perspective(float ar, float z_near, float z_far,
                                 float alpha);
    matrix::Matrix4F orthographic(float left, float right, float top,
                                  float bottom, float z_near, float z_far);

    /// Double version
    matrix::Matrix3D scale2D(double dx, double dy);
    matrix::Matrix3D scale2D(double ds);
    matrix::Matrix4D scale3D(double dx, double dy, double dz);
    matrix::Matrix4D scale3D(double ds);

    matrix::Matrix3D translate2D(double dx, double dy);
    matrix::Matrix3D translate2D(matrix::Vector2D du);
    matrix::Matrix4D translate3D(double dx, double dy, double dz);
    matrix::Matrix4D translate3D(matrix::Vector3D du);

    matrix::Matrix3D rotate2D(double theta);
    matrix::Matrix4D rotate3Dx(double theta);
    matrix::Matrix4D rotate3Dy(double theta);
    matrix::Matrix4D rotate3Dz(double theta);

    matrix::Matrix4D perspective(double ar, double z_near,
                                 double z_far, double alpha);
    matrix::Matrix4D orthographic(double left, double right, double top,
                                  double bottom, double z_near, double z_far);
}; // namespace math::transformation
