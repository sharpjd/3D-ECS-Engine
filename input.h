#pragma once

#include <GLFW/glfw3.h>

class Input {
public:
	Input(GLFWwindow** glfwWindow) : glfwWindow(glfwWindow) {}

	//TODO make a key namespace or something
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
	bool space = false;
	bool shift = false;

	void update() {
		processInput(*glfwWindow);
	}

	void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			w = true;
		else
			w = false;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			a = true;
		else
			a = false;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			s = true;
		else
			s = false;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			d = true;
		else
			d = false;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			space = true;
		else
			space = false;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
			|| glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS
			)
			shift = true;
		else
			shift = false;
	}
private:
	GLFWwindow** glfwWindow;
};