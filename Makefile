CXX = clang++
GLFW = $$(pkg-config --libs glfw3) $$(pkg-config --cflags glfw3)
VULKAN = -L/Users/fahd/VulkanSDK/1.2.198.1/macOS/lib -lvulkan -I/Users/fahd/VulkanSDK/1.2.198.1/macOS/include
GLM = -I/opt/homebrew/Cellar/glm/0.9.9.8/include

out: main.cpp
	${CXX} main.cpp -std=c++17 ${GLFW} ${VULKAN} ${GLM} -o out
