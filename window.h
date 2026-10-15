#pragma once

#include "libs/glad/glad.h" //MUST be included before glfw (glfw checks for it)
#include <GLFW/glfw3.h>

class Window {
public:

	GLFWwindow** glfwWindow;
	const int WIDTH = 800;
	const int HEIGHT = 600;

	bool shouldEnd = false;

	Window(GLFWwindow** glfwWindow) : glfwWindow(glfwWindow) {

	}

	void init() {
		
		initWindow();

		initGLAD();

		initGL();

	}

	void update() {

		if(glfwWindowShouldClose(*glfwWindow)){
			shouldEnd = true;
		}

		glfwPollEvents();
		glfwSwapBuffers(*glfwWindow);

		if (framebufferResized) {
			//TODO do something about it but idc now
			framebufferResized = false;
		}
	}

	void cleanup() {
		glfwDestroyWindow(*glfwWindow);
		glfwTerminate();
	}

	

	void initGL() {
		glViewport(0, 0, WIDTH, HEIGHT);
	}

	int initGLAD() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}
		return 0;
	}

	bool framebufferResized = false;

	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		*glfwWindow = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
		glfwMakeContextCurrent(*glfwWindow);
		glfwSwapInterval(0); //VSYNC off; must be called after makeContextCurrent
		glfwSetWindowUserPointer(*glfwWindow, this);
		glfwSetFramebufferSizeCallback(*glfwWindow, framebufferResizeCallback);
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}
};