// Thanks to: 
// https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html
// https://github.com/adamhlt/DLL-Injector (adamhit)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

DWORD GetProcessByName(const char* lpProcessName)
{
    char lpCurrentProcessName[255];

    PROCESSENTRY32 ProcList{};
    ProcList.dwSize = sizeof(ProcList);

    const HANDLE hProcList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcList == INVALID_HANDLE_VALUE)
        return -1;

    if (!Process32First(hProcList, &ProcList))
        return -1;

    wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

    if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
        return ProcList.th32ProcessID;

    while (Process32Next(hProcList, &ProcList))
    {
        wcstombs_s(nullptr, lpCurrentProcessName, ProcList.szExeFile, 255);

        if (lstrcmpA(lpCurrentProcessName, lpProcessName) == 0)
            return ProcList.th32ProcessID;
    }

    return -1;
}

int main(int argc, char* argv[])
{
    const char *szDLLPathToInject = "./DiscordActivity.dll";
    int nDLLPathLen = lstrlenA(szDLLPathToInject);
    int nTotBytesToAllocate = nDLLPathLen + 1; // including NULL character.

    int processId = GetProcessByName("haloce.exe");
    
    if (processId <= 0) return 1;

    const HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hTargetProcess == INVALID_HANDLE_VALUE)
    {
        printf("An error is occured when trying to open the target process.\n");
        return -1;
    }

    const LPVOID lpPathAddress = VirtualAllocEx(
        hTargetProcess, nullptr, nTotBytesToAllocate,
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    );

    if (lpPathAddress == nullptr)
    {
        printf("An error is occured when trying to allocate memory in the target process.\n");
        return -1;
    }

    const DWORD dwWriteResult = WriteProcessMemory(
        hTargetProcess, lpPathAddress, 
        szDLLPathToInject, nTotBytesToAllocate,
        nullptr
    );

    if (dwWriteResult == 0)
    {
        printf("An error is occured when trying to write the DLL path in the target process.\n");
        return -1;
    }

    const HMODULE hModule = GetModuleHandleA("kernel32.dll");
    if (hModule == INVALID_HANDLE_VALUE || hModule == nullptr)
        return -1;

    const FARPROC lpFunctionAddress = GetProcAddress(hModule, "LoadLibraryA");
    if (lpFunctionAddress == nullptr)
    {
        printf("An error is occured when trying to get \"LoadLibraryA\" address.\n");
        return -1;
    }

    const HANDLE hThreadCreationResult = CreateRemoteThread(
        hTargetProcess, nullptr, 0, 
        (LPTHREAD_START_ROUTINE)lpFunctionAddress, lpPathAddress, 0, 
        nullptr
    );

    if (hThreadCreationResult == INVALID_HANDLE_VALUE)
    {
        printf("An error is occured when trying to create the thread in the target process.\n");
        return -1;
    }

    printf("Code Injected\n");

    return 0;
}