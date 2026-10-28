#pragma once

struct Light {
	glm::vec3 ambient{ 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse{ 0.0f, 0.0f, 0.0f };
	glm::vec3 specular{ 0.0f, 0.0f, 0.0f };
};