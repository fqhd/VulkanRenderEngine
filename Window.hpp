#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
	Window(int width, int height, const char* title);
	void processInput();
	bool shouldClose();
	void close();

private:
	GLFWwindow* window = nullptr;
};