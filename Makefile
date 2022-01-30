CXX = clang++
GLFW = -L/opt/homebrew/Cellar/glfw/3.3.5/lib -lglfw -I/opt/homebrew/Cellar/glfw/3.3.5/include
VULKAN = -L/Users/fahd/VulkanSDK/1.2.198.1/macOS/lib -lvulkan -I/Users/fahd/VulkanSDK/1.2.198.1/macOS/include
GLM = -I/opt/homebrew/Cellar/glm/0.9.9.8/include

out: main.cpp Window.hpp Window.cpp Renderer.hpp Renderer.cpp
	${CXX} main.cpp Window.cpp Renderer.cpp -std=c++17 ${GLFW} ${VULKAN} ${GLM} -o out
