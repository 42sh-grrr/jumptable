#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 0) uniform Ubo {
    mat4 mvp;
    float time;
} ubo;
layout(binding = 1, set = 1) uniform sampler2D u_tex;

void main() {
    // outColor = vec4((sin(ubo.time * 1.5 + 0.1) + 1.) / 2., (sin(ubo.time * 2. + 0.5) + 1.) / 2., (sin(ubo.time * 2.5 + 1.) + 1.) / 2., 1.0);
    // outColor = vec4(v_uv, 0., 1.);
    // outColor = vec4(v_uv, 0., 1.);
    outColor = texture(u_tex, v_uv);
    // outColor = vec4(1.);
}
