#include "renderer.h"

void init_renderer(Renderer* renderer){
	// Initializing vulkan
	init_vulkan(&renderer->vulkan);
}

void render_scene(Renderer* renderer){
	draw_frame(&renderer->vulkan);
}

void destroy_renderer(Renderer* renderer){
	destroy_vulkan(&renderer->vulkan);
}
