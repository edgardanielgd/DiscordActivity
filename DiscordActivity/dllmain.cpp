// dllmain.cpp : Define el punto de entrada de la aplicación DLL.
#include "pch.h"

#define LOG_FILE L"C:\\MyLogFile.txt"

void WriteLog(char* text)
{
    HANDLE hfile = CreateFileW(
        LOG_FILE, GENERIC_WRITE, 
        FILE_SHARE_READ, NULL, 
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
        NULL
    );
    DWORD written;
    WriteFile(hfile, text, strlen(text), &written, NULL);
    WriteFile(hfile, "\r\n", 2, &written, NULL);
    CloseHandle(hfile);
}

HANDLE main_thread;
HMODULE current_hModule;

// Main function
int main() {
    WriteLog("Thread starting");
}

// DLL Load into game process
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        current_hModule = hModule;
        WriteLog("Test, Injected");
        main_thread = CreateThread(
            NULL, 0,
            (LPTHREAD_START_ROUTINE)main,
            NULL, 0, NULL
        );
        if (main_thread == NULL) {
            return FALSE;
        }
        WriteLog("Test, Process created");
        break;
    case DLL_THREAD_ATTACH:
        TerminateThread(main_thread, 0);
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

