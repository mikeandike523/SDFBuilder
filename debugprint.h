#pragma once
#include <Windows.h>
#define CONSOLE_PRINTF_512(...) {char cad[512]; sprintf(cad, __VA_ARGS__);  OutputDebugStringA((LPCSTR)cad);}
#define CONSOLE_PRINTF_2048(...) {char cad[2048]; sprintf(cad, __VA_ARGS__);  OutputDebugStringA((LPCSTR)cad);}