#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform Ubo {
    mat4 mvp;
    float time;
} ubo;

void main() {
    gl_Position = ubo.mvp * vec4(pos, 1.0);
    fragColor = color;
}
