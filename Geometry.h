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
	constexpr int DATA_SIZE =256;
	constexpr int GLOBAL_LOCAL_RATIO =16;

	float sdBox(vec3 p, vec3 b);

	typedef struct triangle { vec3 A; vec3 B; vec3 C; triangle(vec3 a, vec3 b, vec3 c): A(a),B(b),C(c){}
	
	float minx() { return std::min({ A.x,B.x,C.x }); }; float maxx() { return std::max({ A.x,B.x,C.x }); };
	float miny() { return std::min({ A.y,B.y,C.y }); }; float maxy() { return std::max({ A.y,B.y,C.y }); };
	float minz() { return std::min({ A.z,B.z,C.z }); }; float maxz() { return std::max({ A.z,B.z,C.z }); };
	
	} triangle;

	float triangle_sdf(Geometry::triangle,glm::vec3,float);
	vec3 normalize_point(vec3 p, float minx, float maxx, float miny, float maxy, float minz, float maxz, float minrange);

	void init_opencl();
	void quit_opencl();
	void set_axes(float _axisX, float _axisY, float _axisZ);

	std::vector<std::vector<triangle>> chunk_triangles(std::vector<triangle>& triangles, int* last_chunk_size);

	float * get_fields(std::vector<std::vector<triangle>>& chunks, int lastChunkSize, std::vector<vec3> & positions, float tri_thickness);

	mat3 rotXZMat3(float angle);
	mat3 rotZYMat3(float angle);

	namespace octree {
		
		void init(float,float,float,int,int,int,int);
		void populate(std::vector<Geometry::triangle>& triangles);
		std::vector<Geometry::triangle>& query_sector(int,int,int);
		glm::ivec3 get_octant(float x, float y, float z);
		std::vector<Geometry::triangle> & get_brute();
		int get_index(int ix, int iy, int iz);
	}
	
}

