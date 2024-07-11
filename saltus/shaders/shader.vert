#version 450

layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec2 in_uv;

layout(location = 0) out vec3 v_frag_color;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec2 v_uv;

layout(binding = 0) uniform Ubo {
    mat4 mvp;
    float time;
} uniforms;

void main() {
    gl_Position = uniforms.mvp * in_pos;
    v_frag_color = in_color;
    v_normal = in_normal;
    v_uv = in_uv;
}
