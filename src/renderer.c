#include "renderer.h"

void init_renderer(Renderer* renderer){
	// Initializing vulkan
	init_vulkan(&renderer->vulkan);
    MeshData m;
    uint32_t indices[] = {
        0, 1, 2
    };
    m.indices = indices;
    m.numIndices = 3;
    m.numVertices = 3;
    Vertex vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    };
    m.vertices = vertices;

    renderer->mesh = create_mesh(&renderer->vulkan, &m);
}

void render_scene(Renderer* renderer){
	draw_frame(&renderer->vulkan, &renderer->mesh, 1);
}

void destroy_renderer(Renderer* renderer){
	destroy_vulkan(&renderer->vulkan, &renderer->mesh, 1);
}
