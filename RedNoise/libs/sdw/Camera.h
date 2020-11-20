#pragma once

#include <glm/glm.hpp>

struct Camera {
	public:
		glm::vec3 pos = glm::vec3(0.0, 0.0, 4.0);
		glm::mat3 rot = glm::mat3(1.0);
		float f = 2.0;
		float speed = 0.05;
		double rSpeed = glm::radians(0.5);
};