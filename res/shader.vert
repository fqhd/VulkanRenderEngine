#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 pass_uv;

void main() {
    gl_Position = vec4(in_position, 1.0);
	pass_uv = in_uv;
}