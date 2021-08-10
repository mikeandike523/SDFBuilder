#pragma once
#include <cmath>
#include <glm/glm.hpp>
using glm::vec3;


namespace tribox{

	bool triBoxOverlap(glm::vec3 boxcenter, glm::vec3 boxhalfsize, glm::vec3 tv0, glm::vec3 tv1,
	glm::vec3 tv2);
	int pointInsideBox(vec3 A, vec3 boxCenter, vec3 boxRadius);
	int triboxInside(vec3 A, vec3 B, vec3 C, vec3 boxCenter, vec3 boxRadius);

}