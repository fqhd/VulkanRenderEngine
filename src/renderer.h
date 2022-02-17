#ifndef RENDERER_H
#define RENDERER_H

#include "vulkan_helper.h"

typedef struct {
	Vulkan vulkan;
} Renderer;

void init_renderer(Renderer* renderer);
void destroy_renderer(Renderer* renderer);
void render_scene(Renderer* renderer);

#endif