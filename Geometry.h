#pragma once
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
using glm::vec3;
using glm::mat3;
#include <cmath>

namespace Geometry {

	constexpr float pi = 3.141592;
	constexpr int DATA_SIZE = 1024;

	typedef struct triangle { vec3 A; vec3 B; vec3 C; triangle(vec3 a, vec3 b, vec3 c): A(a),B(b),C(c){}
	
	float minx() { return std::min({ A.x,B.x,C.x }); }; float maxx() { return std::max({ A.x,B.x,C.x }); };
	float miny() { return std::min({ A.y,B.y,C.y }); }; float maxy() { return std::max({ A.y,B.y,C.y }); };
	float minz() { return std::min({ A.z,B.z,C.z }); }; float maxz() { return std::max({ A.z,B.z,C.z }); };
	
	} triangle;

	float triangle_sdf(Geometry::triangle,glm::vec3,float);
	vec3 normalize_point(vec3 p, float minx, float maxx, float miny, float maxy, float minz, float maxz, float minrange);

	void init_opencl();
	void quit_opencl();


	std::vector<std::vector<triangle>> chunk_triangles(std::vector<triangle>& triangles, int* last_chunk_size);

	float get_field(std::vector<std::vector<triangle>> & chunks, int lastChunkSize, vec3 position, float tri_thickness);
	float * get_fields(std::vector<std::vector<triangle>>& chunks, int lastChunkSize, std::vector<vec3> & positions, float tri_thickness);

	mat3 rotXZMat3(float angle);
	mat3 rotZYMat3(float angle);
	
}

