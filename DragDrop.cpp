#include "DragDrop.h"
#include <Windows.h>
#include <stdlib.h>
#include <iostream>
#include "debugprint.h"
#include <thread>
#include <shellapi.h>
#include <iostream>
using std::thread;

namespace DragDrop {
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN


	HWND hwnd;
	bool isRunning;
	WNDCLASSEX windowClass;
	void(*cb)(wchar_t* fileName);

	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			isRunning = true;
		}
		break;
		case WM_DESTROY:
		case WM_CLOSE:
		case WM_QUIT:
			isRunning = false;
			PostQuitMessage(0);
			return 0;
			break;
		case WM_DROPFILES:
		{
			HDROP drop = (HDROP)wParam;
			UINT filePathesCount = DragQueryFileW(drop, 0xFFFFFFFF, NULL, 512);//If "0xFFFFFFFF" as the second parameter: return the count of files dropped
			wchar_t* fileName = NULL;
			UINT longestFileNameLength = 0;
			for (UINT i = 0; i < filePathesCount; ++i)
			{
				//If NULL as the third parameter: return the length of the path, not counting the trailing '0'
				UINT fileNameLength = DragQueryFileW(drop, i, NULL, 512) + 1;
				if (fileNameLength > longestFileNameLength)
				{
					longestFileNameLength = fileNameLength;
					fileName = (wchar_t*)realloc(fileName, longestFileNameLength * sizeof(*fileName));
				}
				DragQueryFileW(drop, i, fileName, fileNameLength);

				std::wcout << fileName << std::endl;
			}
			cb(fileName);
			free(fileName);
			DragFinish(drop);
			break;
		}

		default:
			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	bool createWindow(HINSTANCE hInstance, int width, int height, int bpp)
	{
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = wndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = hInstance;
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = NULL;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = L"DragnDropClass";
		windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&windowClass))
		{
			std::cout << "Couldn't register window class" << std::endl;
			return false;
		}

		DWORD      dwExStyle;
		DWORD      dwStyle;
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;

		RECT windowRect;
		windowRect.left = 0;
		windowRect.right = width;
		windowRect.top = 0;
		windowRect.bottom = height;
		if (AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle) == 0)
		{
			std::cout << "Couldn't set the size of the window rectangle" << std::endl;
			return false;
		}

		hwnd = CreateWindowEx(0,
			windowClass.lpszClassName,
			L"Minimal Drag and Drop Application for Windows",
			dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			width,
			height,
			NULL,
			NULL,
			hInstance,
			NULL);

		if (!hwnd)
		{
			std::cout << "Couldn't create window" << std::endl;
			return false;
		}

		DragAcceptFiles(hwnd, TRUE);

		return true;
	}

	

	bool processEvents()
	{
		MSG msg;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return isRunning;
	}


	int init(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR cmdLine,
		int cmdShow, HWND* handle, void(*callback)(wchar_t* fileName))
	{
		cb = callback;
		//AllocConsole();
		//HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		//SetConsoleTitle(L"Minimal Drag and Drop Application for Windows: Output Console");
		//freopen("CONOUT$", "w", stdout);
	//	freopen("CONOUT$", "w", stderr);

		if (!createWindow(hInstance, 640, 480, 32))
		{
		//	system("PAUSE");
			//return 1;
		}

		*handle = hwnd;
		

		return 0;
	}

}
