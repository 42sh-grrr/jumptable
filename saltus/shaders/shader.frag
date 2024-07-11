#version 450

layout(location = 0) in vec3 v_frag_olor;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform Ubo {
    mat4 mvp;
    float time;
    vec3 light_direction;
} uniforms;
layout(set = 1, binding = 0) uniform sampler2D u_tex;

const float AMBIENT = 0.2;
const vec3 LIGHT_DIR = vec3(1., 0., 0.);

void main() {
    out_color = texture(u_tex, v_uv);
    float ambient_strength = 0.05;
    float diffuse_strength = max(dot(v_normal, uniforms.light_direction), 0.);
    out_color *= ambient_strength + diffuse_strength;
    // out_color = vec4((v_normal + 1.) / 2., 1.);
}
