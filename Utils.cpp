#include "Utils.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <cstdlib>
#include <windows.h>
#include <string>
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
namespace Utils {

    void convertStringToLower(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });
    }

    std::string stringToLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return str;
    }

    //https://stackoverflow.com/a/686373/5166365
    float randf() {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        return r;
    }

    long now_millis() {
        SYSTEMTIME time;
        GetSystemTime(&time);
        LONG time_ms = (time.wSecond * 1000) + time.wMilliseconds;
        return time_ms;
    }

    void file_get_contents_512(std::string filename, std::string& contents) {
        FILE* fl = fopen(filename.c_str(), "r");
        char buffer[512];
        while (fgets(buffer, 512, fl) != NULL) {
            contents += std::string(buffer);
        }
        fclose(fl);
    }

    std::string convertWtoA(std::wstring& wide) {

        return std::string(wide.begin(), wide.end());
    }

    //https://stackoverflow.com/a/8969776/5166365
    int StringToWString(std::wstring& ws, const std::string& s)
    {
        std::wstring wsTmp(s.begin(), s.end());

        ws = wsTmp;

        return 0;
    }

    glm::vec3 swapYZ(glm::vec3& toSwap) {
        return glm::vec3(toSwap.xzy);
    }

    glm::vec3 vec3MulElem(glm::vec3& A, glm::vec3& B) {
        return glm::vec3(A.x * B.x, A.y * B.y, A.z * B.z);
    }

    int max2i(int a, int b) { return a > b ? a : b; }
    int max3i(int a, int b, int c) { return max2i(a, max2i(b, c)); }

    float max2f(float a, float b) { return a > b ? a : b; }
    float max3f(float a, float b, float c) { return max2f(a, max2f(b, c)); }



}