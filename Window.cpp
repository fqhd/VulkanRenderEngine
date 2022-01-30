#include "Window.hpp"

Window::Window(int width, int height, const char* title){
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void Window::processInput(){
	glfwPollEvents();
}

bool Window::shouldClose(){
	return glfwWindowShouldClose(window);
}

void Window::close(){
	glfwDestroyWindow(window);

	glfwTerminate();
}