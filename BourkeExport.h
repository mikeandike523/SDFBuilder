#pragma once
#include <string>
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <vector>


namespace BourkeExport
{
	void export_sdf(std::wstring fn, glm::vec3 axes, glm::ivec3 res, float * texels);
	void import_sdf(std::wstring inputFilename, std::vector<float> & texels,int * resxp, int * resyp, int * reszp, float * axisxp, float * axisyp, float * axiszp);
}

