#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "system.h"
#include "registry.h"

#include "comps/camera.h"

static double mouseCallbackXPos = 0.0;
static double mouseCallbackYPos = 0.0;
static double scrollCallbackXOffset = 0.0;
static double scrollCallbackYOffset = 0.0;

class CameraSys : public ISystem {
private:
public:
	CameraSys(EngineContext& ec) : ISystem(ec)
	{
	}

	void start() override {
		Camera camera{};
		auto newEntity = ec.reg.newEntity();
		ec.reg.addComponent(newEntity, camera);

		auto window = *(ec.window.glfwWindow);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
	}

	bool firstMouse = false;
	float lastX = 400, lastY = 300;

	void update() override {
		const float cameraSpeed = 2.5f * ec.time.deltaTime;

		Query q = ec.reg.query<Camera>();
		assert(q.count() == 1);
		for (auto [eh, cam] : q) {
			auto& cameraFront = cam.cameraFront;
			auto& cameraUp = cam.cameraUp;
			auto& yaw = cam.yaw;
			auto& pitch = cam.pitch;

			float xpos = mouseCallbackXPos;
			float ypos = mouseCallbackYPos;

			if (firstMouse) // initially set to true
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
			lastX = xpos;
			lastY = ypos;

			const float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;
			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(direction);

			if (ec.input.w)
				//cam.cameraPos += cameraSpeed * cameraFront; //W = direction of mouse, including up/down
				cam.cameraPos += cameraSpeed * glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
			if (ec.input.s)
				cam.cameraPos -= cameraSpeed * glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
			if (ec.input.a)
				cam.cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			if (ec.input.d)
				cam.cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			if (ec.input.space)
				cam.cameraPos += cameraSpeed * cameraUp;
			if (ec.input.shift)
				cam.cameraPos -= cameraSpeed * cameraUp;

			cam.view = glm::lookAt(cam.cameraPos, cam.cameraPos + cameraFront, cameraUp);

			float& fov = cam.fov;
			float zoomSensitivity = 10000.0f;
			fov -= (float)scrollCallbackYOffset * zoomSensitivity * ec.time.deltaTime;
			scrollCallbackYOffset = 0;
			if (fov < 1.0f)
				fov = 1.0f;
			if (fov > 45.0f)
				fov = 45.0f;
		}
		
	}

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		mouseCallbackXPos = xpos;
		mouseCallbackYPos = ypos;
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		scrollCallbackXOffset = xoffset;
		scrollCallbackYOffset = yoffset;
	}
};