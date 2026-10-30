#pragma once

struct Light {
	glm::vec3 ambient{ 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse{ 0.0f, 0.0f, 0.0f };
	glm::vec3 specular{ 0.0f, 0.0f, 0.0f };
};

struct DirectionalLight {
	glm::vec3 direction{ 0.0f,0.0f,0.0f };
	glm::vec3 ambient{ 0.0f,0.0f,0.0f };
	glm::vec3 diffuse{ 0.0f,0.0f,0.0f };
	glm::vec3 specular{ 0.0f,0.0f,0.0f };
};

struct PointLight {
	float constant = 0.0f;
	float linear = 0.0f;
	float quadratic = 0.0f;

	glm::vec3 ambient{ 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse{ 0.0f, 0.0f, 0.0f };
	glm::vec3 specular{ 0.0f, 0.0f, 0.0f };
};