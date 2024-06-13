#include <iostream>
#include <array>
#include <fstream>
#include <matrix/matrix.hh>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <saltus/renderer.hh>
#include <saltus/material.hh>
#include <unistd.h>
#include "saltus/mesh.hh"
#include "saltus/vertex_attribute.hh"

static std::vector<char> read_full_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("failed to open file!");

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    return buffer;
}

int main()
{
    auto window = saltus::WindowBuilder()
        .title("bite")
        .build();
    auto renderer = saltus::Renderer::create(window);

    saltus::MeshCreateInfo mesh_info{};
    mesh_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
    mesh_info.vertex_count = 3;
    mesh_info.vertex_attributes.push_back({
        .name = "position",
        .type = saltus::VertexAttributeType::Vec2f,
        .data = saltus::to_bytearray(std::vector<float>{
             0.0f,-0.5f,
             0.5f, 0.5f,
            -0.5f, 0.0f
        }),
    });
    mesh_info.vertex_attributes.push_back({
        .name = "color",
        .type = saltus::VertexAttributeType::Vec3f,
        .data = saltus::to_bytearray(std::vector<float>{
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        }),
    });
    auto mesh = renderer->create_mesh(mesh_info);

    saltus::MaterialCreateInfo material_info{};
    material_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
    material_info.vertex_shader = renderer->create_shader({
        .kind = saltus::ShaderKind::Vertex,
        .source_code = read_full_file("saltus/shaders/shader.vert.spv"),
    });
    material_info.fragment_shader = renderer->create_shader({
        .kind = saltus::ShaderKind::Fragment,
        .source_code = read_full_file("saltus/shaders/shader.frag.spv"),
    });
    material_info.vertex_attributes.push_back({
        .location = 0,
        .name = "position",
        .type = saltus::VertexAttributeType::Vec2f,
    });
    material_info.vertex_attributes.push_back({
        .location = 1,
        .name = "color",
        .type = saltus::VertexAttributeType::Vec3f,
    });
    auto material = renderer->create_material(material_info);

    std::vector<std::shared_ptr<saltus::InstanceGroup>> instance_groups;
    instance_groups.push_back(renderer->create_instance_group({
        .material = material,
        .mesh = mesh,
    }));
    saltus::RenderInfo render_info {
        .instance_groups = instance_groups,
    };

    for (;;)
    {
        auto event = window.wait_event();
        if (!event)
            continue;

        if (dynamic_cast<saltus::WindowExposeEvent*>(&*event))
        {
            std::cout << "Rendering...\n";
            renderer->render(render_info);
            std::cout << "Finished rendering !\n";
            continue;
        }

        if (dynamic_cast<saltus::WindowCloseRequestEvent*>(&*event))
            break;
    }

    renderer->wait_for_idle();

    std::cout << "Bye bye !\n";

    return 0;
}
