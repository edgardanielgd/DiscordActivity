#include <windows.h>
#include <winerror.h>
#include <psapi.h>

void Attack()
{
    char szProcessName[128];
    GetModuleBaseNameA(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
    MessageBoxA(NULL, "BOOM!", szProcessName, MB_OK);
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        Attack();
        break;

    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (lpvReserved != nullptr)
        {
            break; // do not do cleanup if process termination scenario
        }
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}