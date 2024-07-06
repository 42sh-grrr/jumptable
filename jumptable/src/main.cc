#include <iostream>
#include <fstream>
#include <matrix/matrix.hh>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <saltus/renderer.hh>
#include <saltus/material.hh>
#include <unistd.h>
#include <logger/logger.hh>
#include <matrix/vector.hh>
#include <saltus/buffer.hh>
#include <saltus/byte_array.hh>
#include <saltus/mesh.hh>
#include <saltus/vertex_attribute.hh>

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
    std::cout << "Creating window...\n";
    auto window = saltus::WindowBuilder()
        .title("bite")
        .build();
    std::cout << "Creating renderer...\n";
    auto renderer = saltus::Renderer::create(window);
    std::cout << "Creating rendering data...\n";

    auto bind_group_layout = renderer->create_bind_group_layout({
        .bindings = {
            {
                .type = saltus::BindingType::UniformBuffer,
                .count = 1,
                .binding_id = 0,
            }
        }
    });
    auto bind_group = renderer->create_bind_group({
        .layout = bind_group_layout,
    });

    std::vector<float> uniform_data = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    auto uniform_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages().with_uniform(),
        .access_hint = saltus::BufferAccessHint::Dynamic,
        .size = uniform_data.size() * sizeof(float),
        .data = reinterpret_cast<uint8_t*>(uniform_data.data()),
    });

    bind_group->set_binding(0, uniform_buffer);

    saltus::MeshCreateInfo mesh_info{};
    mesh_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
    mesh_info.vertex_count = 3;
    mesh_info.vertex_attributes.push_back({
        .name = "position",
        .type = saltus::VertexAttributeType::Vec2f,
        .buffer = renderer->create_buffer(saltus::buffer_from_byte_array(
            saltus::BufferUsages{}.with_vertex(),
            saltus::BufferAccessHint::Static,
            saltus::to_bytearray(std::vector<float>{
                 0.0f,-0.5f,
                 0.5f, 0.5f,
                -0.5f, 0.5f
            })
        )),
    });
    mesh_info.vertex_attributes.push_back({
        .name = "color",
        .type = saltus::VertexAttributeType::Vec3f,
        .buffer = renderer->create_buffer(saltus::buffer_from_byte_array(
            saltus::BufferUsages{}.with_vertex(),
            saltus::BufferAccessHint::Static,
            saltus::to_bytearray(std::vector<float>{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f
            })
        )),
    });
    auto mesh = renderer->create_mesh(mesh_info);

    saltus::MaterialCreateInfo material_info{};
    material_info.bind_group_layouts.push_back(bind_group_layout);
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

    auto instance_group = renderer->create_instance_group({
        .material = material,
        .mesh = mesh,
        .bind_groups = { bind_group }
    });

    auto render = [&renderer,&instance_group]() {
        std::cout << "Rendering...\n";
        matrix::Vector4F vec;
        vec.x() = 0.f;
        vec.y() = 0.f;
        vec.z() = 0.f;
        vec.w() = 1.f;
        renderer->render({
            .instance_groups = { instance_group },
            .clear_color = vec,
        });
        std::cout << "Finished rendering !\n";
    };

    // Initial render because sometimes no expose is emited at the begining
    render();

    std::cout << "Starting event loop!\n";
    for (;;)
    {
        auto event = window.wait_event();
        if (!event)
            continue;

        if (dynamic_cast<saltus::WindowExposeEvent*>(&*event))
        {
            render();
            continue;
        }

        if (dynamic_cast<saltus::WindowCloseRequestEvent*>(&*event))
            break;
    }

    renderer->wait_for_idle();

    std::cout << "Bye bye !\n";

    return 0;
}
