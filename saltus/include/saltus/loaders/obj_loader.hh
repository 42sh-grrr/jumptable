#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <matrix/vector.hh>

namespace saltus::loaders::obj
{
    struct Material
    {
        std::string      name                      = "unnamed";
        matrix::Vector3F ambient_ligting           = {{ 1., 1., 1. }};
        matrix::Vector3F diffuse_color             = {{ 1., 1., 1. }};
        matrix::Vector3F specular_color            = {{ 0., 0., 0. }};
        float            specular_exponent         = 0.;
        float            dissolved                 = 1.;
        matrix::Vector3F transmission_filter_color = {{ 1., 1., 1. }};
        float            refraction_index          = 1;

        int illum;
    };

    struct Object
    {
        std::optional<std::string> name;
        std::vector<matrix::Vector4F> positions;
        std::vector<matrix::Vector3F> texture_coordinates;
        std::vector<matrix::Vector3F> normals;
        std::vector<matrix::Vector3F> colors;

        std::vector<uint32_t> indices;

        std::optional<size_t> material_index;
    };

    struct LoadedObj
    {
        std::vector<Object> objects;
        std::vector<Material> materials;
    };

    LoadedObj load_obj(const std::string &file);
} // namespace saltus
