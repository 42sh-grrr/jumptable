#include "saltus/obj_loader.hh"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace saltus::obj
{
    std::unordered_map<std::string, Material> load_mtl(std::string& filename)
    {
        std::unordered_map<std::string, Material> materials;
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open material file: " + filename);
        }

        Material current_material;
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "newmtl")
            {
                if (!current_material.name.empty()) {
                    materials[current_material.name] = current_material;
                }
                current_material = Material();
                iss >> current_material.name;
            }
            else if (token == "Ka")
            {
                iss >> current_material.ambient_ligting[0] 
                    >> current_material.ambient_ligting[1] 
                    >> current_material.ambient_ligting[2];
            }
            else if (token == "Kd")
            {
                iss >> current_material.diffuse_color[0] 
                    >> current_material.diffuse_color[1] 
                    >> current_material.diffuse_color[2];
            }
            else if (token == "Ks")
            {
                iss >> current_material.specular_color[0] 
                    >> current_material.specular_color[1] 
                    >> current_material.specular_color[2];
            }
            else if (token == "Ns")
            {
                iss >> current_material.specular_exponent;
            }
            else if (token == "d" || token == "Tr")
            {
                iss >> current_material.dissolved;
            }
            else if (token == "Tf")
            {
                iss >> current_material.transmission_filter_color[0] 
                    >> current_material.transmission_filter_color[1] 
                    >> current_material.transmission_filter_color[2];
            }
            else if (token == "Ni")
            {
                iss >> current_material.refraction_index;
            }
            else if (token == "illum")
            {
                iss >> current_material.illum;
            }
        }

        if (!current_material.name.empty()) {
            materials[current_material.name] = current_material;
        }

        return materials;
    }

    LoadedObj load_obj(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open OBJ file: " + filepath);
        }

        LoadedObj result{};
        Object current_object{};
        std::unordered_map<std::string, size_t> material_map;
        std::vector<matrix::Vector4F> temp_positions;
        std::vector<matrix::Vector3F> temp_colors;
        std::vector<matrix::Vector3F> temp_texture_coords;
        std::vector<matrix::Vector3F> temp_normals;

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "o" || token == "g")
            {
                if (!current_object.positions.empty())
                {
                    result.objects.push_back(std::move(current_object));
                    current_object = Object();
                }

                std::string obj_name;
                iss >> obj_name;
                current_object.name = obj_name;
            }
            else if (token == "v")
            {
                std::vector<float> vals;
                float val;
                while (iss >> val)
                    vals.push_back(val);

                size_t pos_size = vals.size() <= 4 ? vals.size() : vals.size() - 3;
                matrix::Vector4F v = {{0., 0., 0., 1.}};
                for (size_t i = 0; i < pos_size; i++)
                    v[i] = vals[i];
                temp_positions.push_back(v);

                if (vals.size() > 4) {
                    matrix::Vector3F color = {{vals[pos_size + 0], vals[pos_size + 1], vals[pos_size + 2]}};
                    temp_colors.push_back(color);
                }
            }
            else if (token == "vt")
            {
                matrix::Vector3F vt;
                iss >> vt[0] >> vt[1];
                if (iss >> vt[2]) {} else { vt[2] = 0.0f; }
                temp_texture_coords.push_back(vt);
            }
            else if (token == "vn")
            {
                matrix::Vector3F vn;
                iss >> vn[0] >> vn[1] >> vn[2];
                temp_normals.push_back(vn);
            }
            else if (token == "f")
            {
                std::string vertex;
                int face_vertex_index = 0;
                while (iss >> vertex)
                {
                    if ((++face_vertex_index) == 4)
                        throw std::runtime_error("Obj file has non-triangle faces which isn't supported");

                    std::istringstream vertex_stream(vertex);
                    std::string index_str;
                    std::array<std::optional<int>, 3> indices;

                    for (int i = 0; i < 3; ++i)
                    {
                        if (std::getline(vertex_stream, index_str, '/') && !index_str.empty())
                            indices[i] = std::stoi(index_str) - 1;
                    }

                    current_object.positions
                        .push_back(temp_positions.at(indices[0].value()));
                    if (static_cast<int>(temp_colors.size()) > indices[0].value())
                        current_object.colors
                            .push_back(temp_colors.at(indices[0].value()));

                    if (indices[1].has_value())
                        current_object.texture_coordinates
                            .push_back(temp_texture_coords.at(indices[1].value()));

                    if (indices[2].has_value())
                        current_object.normals
                            .push_back(temp_normals.at(indices[2].value()));
                }
                if (face_vertex_index != 3)
                    throw std::runtime_error("Obj file has non-triangle faces which isn't supported");
            }
            else if (token == "usemtl")
            {
                std::string material_name;
                iss >> material_name;
                auto it = material_map.find(material_name);
                if (it != material_map.end())
                    current_object.material_index = it->second;
            }
            else if (token == "mtllib")
            {
                std::string mtl_filename;
                iss >> mtl_filename;
                std::filesystem::path obj_path(filepath);
                std::filesystem::path mtl_path = obj_path.parent_path() / mtl_filename;
                std::string mtl_path_str = mtl_path.string();
                auto materials = load_mtl(mtl_path_str);
                for (const auto& [name, material] : materials) {
                    result.materials.push_back(material);
                    material_map[name] = result.materials.size() - 1;
                }
            }
        }

        if (!current_object.positions.empty())
        {
            result.objects.push_back(std::move(current_object));
        }

        return result;
    }
} // namespace saltus
