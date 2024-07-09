#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iterator>
#include <matrix/matrix.hh>
#include <numbers>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <saltus/renderer.hh>
#include <saltus/material.hh>
#include <thread>
#include <unistd.h>
#include <logger/logger.hh>
#include <matrix/vector.hh>
#include <saltus/buffer.hh>
#include <saltus/byte_array.hh>
#include <saltus/mesh.hh>
#include <saltus/vertex_attribute.hh>
#include <variant>
#include "math/transformation.hh"
#include "quick_event_queue.hh"
#include "saltus/instance_group.hh"
#include "saltus/loaders/obj_loader.hh"

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

struct ExitEvent
{ };

struct ShouldRenderEvent
{ };

struct RenderFinishedEvent
{ };

using QuickEvent = std::variant<
    std::shared_ptr<saltus::WindowEvent>,
    ExitEvent, ShouldRenderEvent, RenderFinishedEvent
>;

struct SharedData
{
    QuickEventQueue<QuickEvent> events;
};

std::shared_ptr<saltus::Mesh>
obj_object_to_mesh(saltus::Renderer *renderer, saltus::loaders::obj::Object object)
{
    size_t vertex_count = object.indices.size();

    if (object.colors.empty())
    {
        std::generate_n(
            std::back_insert_iterator(object.colors),
            vertex_count,
            [](){ return matrix::Vector3F({ 1.f, 1.f, 1.f }); }
        );
    }
    if (object.normals.empty())
    {
        std::generate_n(
            std::back_insert_iterator(object.normals),
            vertex_count,
            [](){ return matrix::Vector3F({ 0.f, 0.f, 0.f }); }
        );
    }
    std::vector<matrix::Vector2F> texcoords;
    texcoords.reserve(object.texture_coordinates.size());
    std::transform(
        object.texture_coordinates.cbegin(), object.texture_coordinates.cend(),
        std::back_insert_iterator<std::vector<matrix::Vector2F>>(texcoords),
        [](matrix::Vector3F v) -> matrix::Vector2F {
            return matrix::Vector2F({v.x(), v.y()});
        }
    );
    if (texcoords.empty())
    {
        std::generate_n(
            std::back_insert_iterator(texcoords),
            vertex_count,
            [](){ return matrix::Vector2F({ 0.f, 0.f }); }
        );
    }

    saltus::MeshCreateInfo mesh_info{};
    mesh_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
    mesh_info.vertex_count = vertex_count;
    mesh_info.index_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_index(),
        .access_hint = saltus::BufferAccessHint::Static,
        .size = object.indices.size() * sizeof(uint32_t),
        .data = reinterpret_cast<uint8_t*>(object.indices.data()),
    });
    mesh_info.index_format = saltus::MeshIndexFormat::UInt32;
    mesh_info.vertex_attributes.push_back({
        .name = "position",
        .type = saltus::VertexAttributeType::Vec4f,
        .buffer = renderer->create_buffer({
            .usages = saltus::BufferUsages{}.with_vertex(),
            .access_hint = saltus::BufferAccessHint::Static,
            .size = object.positions.size() * sizeof(float) * 4,
            .data = reinterpret_cast<uint8_t*>(object.positions.data()),
        }),
    });
    mesh_info.vertex_attributes.push_back({
        .name = "color",
        .type = saltus::VertexAttributeType::Vec3f,
        .buffer = renderer->create_buffer({
            .usages = saltus::BufferUsages{}.with_vertex(),
            .access_hint = saltus::BufferAccessHint::Static,
            .size = object.colors.size() * sizeof(float) * 3,
            .data = reinterpret_cast<uint8_t*>(object.colors.data()),
        }),
    });
    mesh_info.vertex_attributes.push_back({
        .name = "normal",
        .type = saltus::VertexAttributeType::Vec3f,
        .buffer = renderer->create_buffer({
            .usages = saltus::BufferUsages{}.with_vertex(),
            .access_hint = saltus::BufferAccessHint::Static,
            .size = object.normals.size() * sizeof(float) * 3,
            .data = reinterpret_cast<uint8_t*>(object.normals.data()),
        }),
    });
    mesh_info.vertex_attributes.push_back({
        .name = "uvs",
        .type = saltus::VertexAttributeType::Vec2f,
        .buffer = renderer->create_buffer({
            .usages = saltus::BufferUsages{}.with_vertex(),
            .access_hint = saltus::BufferAccessHint::Static,
            .size = texcoords.size() * sizeof(float) * 2,
            .data = reinterpret_cast<uint8_t*>(texcoords.data()),
        }),
    });
    return renderer->create_mesh(mesh_info);
}

void render_thread_fn(
    saltus::Renderer *renderer,
    SharedData *shared_data
) {
    logger::info() << "Loading model...\n";
    // auto model = saltus::obj::load_obj("assets/cube.obj");
    // auto model = saltus::loaders::obj::load_obj("assets/sponza/sponzaobj.obj");
    auto model = saltus::loaders::obj::load_obj("assets/lion.obj");
    logger::info() << "Model loaded !\n";

    auto receiver = shared_data->events.subscribe();

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

        // time
        0.0f,
    };
    auto uniform_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_uniform(),
        .access_hint = saltus::BufferAccessHint::Dynamic,
        .size = uniform_data.size() * sizeof(float),
        .data = reinterpret_cast<uint8_t*>(uniform_data.data()),
    });

    bind_group->set_binding(0, uniform_buffer);

    std::vector<std::shared_ptr<saltus::InstanceGroup>> instance_groups;

    saltus::MaterialCreateInfo material_info{};
    material_info.bind_group_layouts.push_back(bind_group_layout);
    material_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
    material_info.cull_mode = saltus::MaterialCullMode::None;
    material_info.vertex_shader = renderer->create_shader({
        .kind = saltus::ShaderKind::Vertex,
        .source_code = read_full_file("build/saltus/shaders/shader.vert.spv"),
    });
    material_info.fragment_shader = renderer->create_shader({
        .kind = saltus::ShaderKind::Fragment,
        .source_code = read_full_file("build/saltus/shaders/shader.frag.spv"),
    });
    material_info.vertex_attributes.push_back({
        .location = 0,
        .name = "position",
        .type = saltus::VertexAttributeType::Vec4f,
    });
    material_info.vertex_attributes.push_back({
        .location = 1,
        .name = "color",
        .type = saltus::VertexAttributeType::Vec3f,
    });
    material_info.vertex_attributes.push_back({
        .location = 2,
        .name = "normal",
        .type = saltus::VertexAttributeType::Vec3f,
    });
    material_info.vertex_attributes.push_back({
        .location = 3,
        .name = "uvs",
        .type = saltus::VertexAttributeType::Vec2f,
    });
    auto material = renderer->create_material(material_info);

    for (auto &object : model.objects)
    {
        auto mesh = obj_object_to_mesh(renderer, object);
        instance_groups.push_back(renderer->create_instance_group({
            .material = material,
            .mesh = mesh,
            .bind_groups = { bind_group }
        }));
    }

    matrix::Matrix4F mvp_matrix;

    auto start_t = std::chrono::high_resolution_clock::now();
    auto last_t = std::chrono::high_resolution_clock::now();
    auto render = [&]() {
        auto microseconds_time = (std::chrono::high_resolution_clock::now() - start_t);
        float time = std::chrono::duration_cast<std::chrono::microseconds>(microseconds_time).count() / 1.e6f;
        uniform_data[16] = time;

        auto buffsize = renderer->framebuffer_size();
        float ar = static_cast<float>(buffsize.x()) / buffsize.y();
        mvp_matrix = matrix::identity<float, 4>();

        mvp_matrix *= math::transformation::perspective(
            ar, 0.001f, 100.f, std::numbers::pi_v<float> / 2.f
        );
        mvp_matrix *= math::transformation::translate3D(
            0.f, 0.5f, 7.5f
        );
        mvp_matrix *= math::transformation::rotate3Dy(time);
        mvp_matrix *= math::transformation::scale3D(
            1.f, -1.f, 1.f
        );
        mvp_matrix = mvp_matrix.transpose();

        memcpy(uniform_data.data(), &mvp_matrix, sizeof(float) * 16);
        
        uniform_buffer->write(reinterpret_cast<uint8_t*>(uniform_data.data()));

        logger::debug() << "Rendering...\n";
        renderer->render({
            .instance_groups = instance_groups,
            .clear_color = matrix::Vector4F{{ 0., 0., 0., 1. }},
        });
        logger::debug() << "Finished rendering !\n";
        auto elapsed = std::chrono::high_resolution_clock::now() - last_t;
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
        logger::debug() << "Elapsed: " << micros << " (" << std::setprecision(2) << std::fixed << (1.e6 / micros.count()) << ") fps\n";

        last_t = std::chrono::high_resolution_clock::now();
        shared_data->events.send(RenderFinishedEvent {});
    };

    while (true)
    {
        auto event = receiver->poll_recv();
        QuickEvent *event_ptr = event ? &event.value() : nullptr;
        if (std::get_if<ExitEvent>(event_ptr))
            break;

        render();
    }

    renderer->wait_for_idle();
}

void events_thread_fn(
    saltus::Window *window,
    SharedData *shared_data
) {
    for (;;)
    {
        auto event = window->wait_event();
        if (!event)
            continue;

        std::shared_ptr<saltus::WindowEvent> shared_event = std::move(event);
        shared_data->events.send(shared_event);

        if (dynamic_cast<saltus::WindowExposeEvent*>(&*shared_event))
            shared_data->events.send(ShouldRenderEvent { });

        if (dynamic_cast<saltus::WindowCloseRequestEvent*>(&*shared_event))
            break;
    }
    shared_data->events.send(ExitEvent { });
    logger::info() << "Hi!\n";
}

int main()
{
    logger::info() << "Creating window...\n";
    auto window = saltus::WindowBuilder()
        .title("bite")
        .build();
    logger::info() << "Creating renderer...\n";
    auto renderer = saltus::Renderer::create({
        .window = window,
        .target_present_mode = saltus::RendererPresentMode::VSync,
        // .target_present_mode = saltus::RendererPresentMode::Immediate,
    });
    logger::info() << "Renderer presentaition mode: " << renderer->current_present_mode() << "\n";
    logger::info() << "Creating rendering data...\n";

    logger::info() << "Starting!\n";

    SharedData shared_data {
        .events = QuickEventQueue<QuickEvent>(),
    };

    std::thread render_thread(render_thread_fn, &*renderer, &shared_data);
    std::thread events_thread(events_thread_fn, &window, &shared_data);
    render_thread.join();
    events_thread.join();

    renderer->wait_for_idle();

    logger::info() << "Bye bye !\n";

    return 0;
}
