#pragma once
#include <windows.h>
#include <string.h>
#include <iostream>
using namespace std; 


//vscode is mistakenly labeling errors due to type conflicts (between windows string types) which are not reported by the compiler


namespace openfilename{
    // Returns an empty string if dialog is canceled
    wstring openfilename(const wchar_t *filter = L"STL Files (*.stl*)\0*.stl\0", HWND owner = NULL) {
    OPENFILENAME ofn;
    wchar_t fileName[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = (LPCWSTR)filter;
    ofn.lpstrFile = (LPWSTR)fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"";

    wstring fileNameStr;

    if ( GetOpenFileName(&ofn) )
        fileNameStr = std::wstring(fileName);

    return fileNameStr;
    }
}

