#ifndef UTILS_H
#define UTILS_H
#include <algorithm>
#include <cctype>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <string>
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>

namespace Utils{
    void convertStringToLower(std::string& str);
    std::string stringToLower(std::string str);
    float randf();
    long now_millis();
    void file_get_contents_512(std::string filename, std::string& contents);
    std::string convertWtoA(std::wstring& wide);
    glm::vec3 swapYZ(glm::vec3& toSwap);
    glm::vec3 vec3MulElem(glm::vec3& A, glm::vec3& B);
    int StringToWString(std::wstring& ws, const std::string& s);
    

}
#endif

