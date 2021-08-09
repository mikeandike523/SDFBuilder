#pragma once
#include <Windows.h>
namespace DragDrop {

	int init(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR cmdLine,
		int cmdShow, HWND* handle, void(*callback)(wchar_t * fileName));
	bool processEvents();
}

