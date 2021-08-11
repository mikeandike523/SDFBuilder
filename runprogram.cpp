#include <Windows.h>
#include <string>

//https://stackoverflow.com/a/15440094/5166365

    std::string convertWtoA(std::wstring& wide) {

        return std::string(wide.begin(), wide.end());
    }


VOID startup(LPCTSTR lpApplicationName, LPWSTR cmdline)
{
   // additional information
   STARTUPINFO si;     
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

  // start the program up
  CreateProcess( lpApplicationName,   // the path
    (LPSTR)convertWtoA(std::wstring(cmdline)).c_str(),        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow)
{
 //   system("Debug\\SDFBuilder.exe");
    startup((LPCTSTR)"Debug\\SDFBuilder.exe",lpCmdLine);
}