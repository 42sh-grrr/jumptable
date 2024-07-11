#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iterator>
#include <matrix/matrix.hh>
#include <memory>
#include <numbers>
#include <saltus/window.hh>
#include <saltus/window_events.hh>
#include <saltus/renderer.hh>
#include <saltus/shader_pack.hh>
#include <stdexcept>
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
#include "saltus/bind_group.hh"
#include "saltus/bind_group_layout.hh"
#include "saltus/image.hh"
#include "saltus/instance_group.hh"
#include "saltus/loaders/obj_loader.hh"
#include "saltus/loaders/tga_loader.hh"
#include "saltus/sampler.hh"

using namespace std::chrono_literals;

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

struct Material
{
    std::shared_ptr<saltus::ShaderPack> shader_pack;
    std::shared_ptr<saltus::BindGroup> bind_group;
};

struct UniformData
{
    matrix::Matrix4F mvp;
    float time;
    float padding1[3];
    // In shader this is a vector3 but it will have a float of padding anyways
    matrix::Vector4F light_direction;
};

std::unique_ptr<Material>
obj_material_to_material(
    saltus::Renderer *renderer,
    saltus::loaders::obj::Material &obj_material,
    std::shared_ptr<saltus::ShaderPack> &shader_pack,
    std::shared_ptr<saltus::BindGroupLayout> &layout
) {
    std::string texpath = obj_material.diffuse_map.empty() ? "assets/default.tga" : obj_material.diffuse_map;

    logger::debug() << "Loading texture " << texpath << "\n";
    auto tgaimage =
        saltus::loaders::tga::load_tga_image(texpath);
    logger::debug() << "Read! " << tgaimage.width << "x" << tgaimage.height << " @ " << tgaimage.bytesPerPixel << "\n";

    std::vector<uint8_t> new_pixels;
    new_pixels.resize(tgaimage.width*tgaimage.height*4, 255);
    if (tgaimage.bytesPerPixel != 4)
    {
        for (size_t i = 0; i < tgaimage.width*tgaimage.height; i++)
        {
            memcpy(
                new_pixels.data() + i * 4,
                tgaimage.data.data() + i * tgaimage.bytesPerPixel,
                tgaimage.bytesPerPixel
            );
        }
    }
    else {
        memcpy(new_pixels.data(), tgaimage.data.data(), tgaimage.data.size());
    }

    logger::debug() << "Converted!\n";
    uint32_t max_mipmaps = saltus::max_image_mip_levels({{ tgaimage.width, tgaimage.height, 1 }});
    logger::debug() << "Using " << max_mipmaps << " mips\n";
    auto image = renderer->create_image({
        .dimensions = {{ tgaimage.width, tgaimage.height, 1 }},
        .mip_levels = max_mipmaps,
        .usages = saltus::ImageUsages{}.with_sampled(),
        .format = {
            .pixel_format = saltus::ImagePixelFormat::BGRA,
            .data_type = saltus::ImageDataType::srgb8,
        },
        .initial_data = new_pixels.data(),
    });
    logger::debug() << "Uploaded!\n";

    saltus::SamplerCreateInfo sampler_info{};
    sampler_info.min_lod = 0.;
    sampler_info.max_lod = max_mipmaps / 1.f;
    sampler_info.mip_lod_bias = 0.;
    if (obj_material.diffuse_map.empty())
    {
        sampler_info.mag_filter = saltus::SamplerFilter::Nearest;
        sampler_info.min_filter = saltus::SamplerFilter::Nearest;
    }
    auto sampler = renderer->create_sampler(sampler_info);

    auto texture = renderer->create_texture({
        .image = image,
        .sampler = sampler
    });

    auto bind_group = renderer->create_bind_group({ .layout = layout });
    bind_group->set_binding(0, texture);

    std::unique_ptr<Material> material = std::make_unique<Material>();
    material->shader_pack = shader_pack;
    material->bind_group = bind_group;
    return material;
}

std::vector<std::shared_ptr<saltus::InstanceGroup>>
obj_object_to_instance_group(
    saltus::Renderer *renderer,
    saltus::loaders::obj::Object &object,
    std::shared_ptr<saltus::BindGroup> global_bind_group,
    std::vector<std::unique_ptr<Material>> &materials
) {
    size_t vertex_count = object.positions.size();

    if (object.colors.empty())
    {
        object.colors.resize(vertex_count, matrix::Vector3F({ 1.f, 1.f, 1.f }));
    }
    if (object.normals.empty())
    {
        object.normals.resize(vertex_count, matrix::Vector3F({ 0.f, 0.f, 0.f }));
    }
    std::vector<matrix::Vector2F> texcoords;
    std::transform(
        object.texture_coordinates.cbegin(), object.texture_coordinates.cend(),
        std::back_insert_iterator<std::vector<matrix::Vector2F>>(texcoords),
        [](matrix::Vector3F v) -> matrix::Vector2F {
            return matrix::Vector2F({v.x(), v.y()});
        }
    );
    if (texcoords.empty())
    {
        texcoords.resize(vertex_count, matrix::Vector2F({ 0.f, 0.f }));
    }

    auto positions_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_vertex(),
        .access_hint = saltus::BufferAccessHint::Static,
        .size = object.positions.size() * sizeof(float) * 4,
        .data = reinterpret_cast<uint8_t*>(object.positions.data()),
    });
    auto colors_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_vertex(),
        .access_hint = saltus::BufferAccessHint::Static,
        .size = object.colors.size() * sizeof(float) * 3,
        .data = reinterpret_cast<uint8_t*>(object.colors.data()),
    });
    auto normals_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_vertex(),
        .access_hint = saltus::BufferAccessHint::Static,
        .size = object.normals.size() * sizeof(float) * 3,
        .data = reinterpret_cast<uint8_t*>(object.normals.data()),
    });
    auto uvs_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_vertex(),
        .access_hint = saltus::BufferAccessHint::Static,
        .size = texcoords.size() * sizeof(float) * 2,
        .data = reinterpret_cast<uint8_t*>(texcoords.data()),
    });

    std::vector<std::shared_ptr<saltus::InstanceGroup>> instance_groups;
    for (const auto &group : object.groups)
    {
        saltus::MeshCreateInfo mesh_info{};
        mesh_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
        mesh_info.vertex_count = group.indices.size();

        std::vector<uint16_t> indices16;
        if (object.positions.size() < 65535)
        {
            indices16.assign(group.indices.cbegin(), group.indices.cend());
            mesh_info.index_format = saltus::MeshIndexFormat::UInt16;
            mesh_info.index_buffer = renderer->create_buffer({
                .usages = saltus::BufferUsages{}.with_index(),
                .access_hint = saltus::BufferAccessHint::Static,
                .size = indices16.size() * sizeof(uint16_t),
                .data = reinterpret_cast<const uint8_t*>(indices16.data()),
            });
        }
        else {
            mesh_info.index_format = saltus::MeshIndexFormat::UInt32;
            mesh_info.index_buffer = renderer->create_buffer({
                .usages = saltus::BufferUsages{}.with_index(),
                .access_hint = saltus::BufferAccessHint::Static,
                .size = group.indices.size() * sizeof(uint32_t),
                .data = reinterpret_cast<const uint8_t*>(group.indices.data()),
            });
        }

        mesh_info.vertex_attributes.push_back({
            .name = "position",
            .type = saltus::VertexAttributeType::Vec4f,
            .buffer = positions_buffer,
        });
        mesh_info.vertex_attributes.push_back({
            .name = "color",
            .type = saltus::VertexAttributeType::Vec3f,
            .buffer = colors_buffer,
        });
        mesh_info.vertex_attributes.push_back({
            .name = "normal",
            .type = saltus::VertexAttributeType::Vec3f,
            .buffer = normals_buffer,
        });
        mesh_info.vertex_attributes.push_back({
            .name = "uvs",
            .type = saltus::VertexAttributeType::Vec2f,
            .buffer = uvs_buffer,
        });

        auto saltus_mesh = renderer->create_mesh(mesh_info);

        auto &material = materials.at(group.material_index.value());

        auto ig = renderer->create_instance_group({
            .shader_pack = material->shader_pack,
            .mesh = saltus_mesh,
            .bind_groups = { global_bind_group, material->bind_group },
        });
        instance_groups.push_back(ig);
    }
    return instance_groups;
}

void render_thread_fn(
    saltus::Renderer *renderer,
    SharedData *shared_data
) {
    logger::info() << "Loading model...\n";
    // auto model = saltus::obj::load_obj("assets/cube.obj");
    auto model = saltus::loaders::obj::load_obj("assets/sponza/sponzaobj.obj");
    // auto model = saltus::loaders::obj::load_obj("assets/lion.obj");
    // auto model = saltus::loaders::obj::load_obj("assets/vase.obj");
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

    auto obj_bind_group_layout = renderer->create_bind_group_layout({
        .bindings = {
            {
                .type = saltus::BindingType::Texture,
                .count = 1,
                .binding_id = 0,
            }
        }
    });

    UniformData uniform_data = {};
    auto uniform_buffer = renderer->create_buffer({
        .usages = saltus::BufferUsages{}.with_uniform(),
        .access_hint = saltus::BufferAccessHint::Dynamic,
        .size = sizeof(uniform_data),
        .data = reinterpret_cast<uint8_t*>(&uniform_data),
    });

    bind_group->set_binding(0, uniform_buffer);

    std::vector<std::shared_ptr<saltus::InstanceGroup>> instance_groups;

    saltus::ShaderPackCreateInfo material_info{};
    material_info.bind_group_layouts.push_back(bind_group_layout);
    material_info.bind_group_layouts.push_back(obj_bind_group_layout);
    material_info.primitive_topology = saltus::PritmitiveTopology::TriangleList;
    material_info.cull_mode = saltus::ShaderPackCullMode::None;
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
    auto shader_pack = renderer->create_shader_pack(material_info);

    logger::info() << "Loading textures...\n";
    std::vector<std::unique_ptr<Material>> materials;
    std::transform(
        model.materials.begin(),
        model.materials.end(),
        std::back_insert_iterator(materials),
        [&](auto &obj_material){
            return obj_material_to_material(
                renderer, obj_material,
                shader_pack, obj_bind_group_layout
            );
        }
    );
    logger::info() << "Finished loading textures\n";
    logger::info() << "Loading objects...\n";

    for (auto &object : model.objects)
    {
        logger::debug() << "Loading object " << object.name.value_or("") << "\n";
        auto new_instances = obj_object_to_instance_group(
            renderer, object,
            bind_group,
            materials
        );
        instance_groups.insert(instance_groups.end(), new_instances.cbegin(), new_instances.cend());
    }
    logger::info() << "Ready !\n";

    matrix::Matrix4F mvp_matrix;

    int frame_counter = 0;
    auto start_t = std::chrono::high_resolution_clock::now();
    auto last_t = std::chrono::high_resolution_clock::now();
    auto last_print = std::chrono::high_resolution_clock::now();
    auto print_interval = 1s;

    auto update = [&]() {
        frame_counter++;

        auto microseconds_time = (std::chrono::high_resolution_clock::now() - start_t);
        float time = std::chrono::duration_cast<std::chrono::microseconds>(microseconds_time).count() / 1.e6f;
        uniform_data.time = time;

        auto rotation = math::transformation::rotate3Dy(time / 2.f);

        auto buffsize = renderer->framebuffer_size();
        float ar = static_cast<float>(buffsize.x()) / buffsize.y();
        mvp_matrix = matrix::identity<float, 4>();

        mvp_matrix *= math::transformation::perspective(
            ar, 0.001f, 100.f, (2.f * std::numbers::pi_v<float>) / 3.f
        );
        mvp_matrix *= math::transformation::translate3D(
            0.f, 0.5f, 0.f
            // 0.f, 0.5f, (std::sin(time / 2.f) + 1.f) / 4.f
            // 0.f, 0.f, (std::sin(time / 2.f) + 1.f) / 4.f
            // 0.f, 0.f, 0.5f
        );
        mvp_matrix *= rotation;
        mvp_matrix *= math::transformation::scale3D(
            1.f, -1.f, 1.f
        );
        mvp_matrix = mvp_matrix.transpose();

        uniform_data.mvp = mvp_matrix;

        matrix::Vector4F light_dir{{1.f, 1.f, 1.f, 1.f}};
        // light_dir = rotation * light_dir;
        float length = std::sqrt(
            light_dir.x()*light_dir.x()+
            light_dir.y()*light_dir.y()+
            light_dir.z()*light_dir.z()
        );
        light_dir.x() /= length;
        light_dir.y() /= length;
        light_dir.z() /= length;
        uniform_data.light_direction = light_dir;
        
        uniform_buffer->write(reinterpret_cast<uint8_t*>(&uniform_data));
    };
    auto render = [&]() {
        update();

        renderer->render({
            .instance_groups = instance_groups,
            .clear_color = matrix::Vector4F{{ 0., 0., 0., 1. }},
        });
        auto elapsed = std::chrono::high_resolution_clock::now() - last_t;
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

        last_t = std::chrono::high_resolution_clock::now();
        shared_data->events.send(RenderFinishedEvent {});

        if (std::chrono::high_resolution_clock::now() - last_print > print_interval)
        {
            logger::info() << "Frame " << frame_counter << " took " << micros << " (" << std::setprecision(2) << std::fixed << (1.e6 / micros.count()) << " fps)\n";
            last_print = std::chrono::high_resolution_clock::now();
        }
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
        .title("jumptable")
        .build();
    logger::info() << "Creating renderer...\n";
    auto renderer = saltus::Renderer::create({
        .window = window,
        .target_present_mode = saltus::RendererPresentMode::VSync,
        // .target_present_mode = saltus::RendererPresentMode::Immediate,
        .msaa_samples = saltus::MsaaSamples::Sample64,
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
