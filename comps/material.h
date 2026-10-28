#pragma once 
#include <glm/fwd.hpp>

struct PhongMaterial {
	glm::vec3 ambient{ 0.0f,0.0f,0.0f };
	glm::vec3 diffuse{ 0.0f,0.0f,0.0f };
	glm::vec3 specular{ 0.0f,0.0f,0.0f };
	float shininess = 0;
};

struct PhongLightMapMaterial {
	int glTexUnit_diffuse;
	int glTexUnit_specular;
	float shininess = 0;
};