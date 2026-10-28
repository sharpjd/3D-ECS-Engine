#pragma once

#include <glm/glm.hpp>

//FYI: glm::vec3 is already only 12 bytes

struct Position {
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
};

struct Scale {
	glm::vec3 scale{ 0.0f, 0.0f, 0.0f };
};

struct Rotation {
	glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
};