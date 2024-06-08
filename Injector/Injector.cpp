
#include <Windows.h>
#include <winerror.h>

#include <stdio.h>

int main(int argc, char* argv[])
{
    // Target process ID must be passed as parameter
    char szDLLPathToInject[] = { "DiscordActivity.dll" };
    int nDLLPathLen = lstrlenA(szDLLPathToInject);
    int nTotBytesToAllocate = nDLLPathLen + 1;

    printf("Injecting to Process: %s\n", argv[1]);

    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, 
        FALSE, 
        atoi(argv[1])
    );

    LPVOID lpHeapBaseAddress1 = VirtualAllocEx(
        hProcess, NULL, nTotBytesToAllocate, MEM_COMMIT, PAGE_READWRITE
    );

    SIZE_T lNumberOfBytesWritten = 0;
    WriteProcessMemory(
        hProcess, lpHeapBaseAddress1, 
        szDLLPathToInject, nTotBytesToAllocate, 
        &lNumberOfBytesWritten
    );

    printf("Process Opened");

    // Get the starting address of the function LoadLibrary which is available in kernal32.dll
    LPTHREAD_START_ROUTINE lpLoadLibraryStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandle(L"Kernel32.dll"), "LoadLibraryA"
    );

    CreateRemoteThread(
        hProcess, NULL, 0, 
        lpLoadLibraryStartAddress, lpHeapBaseAddress1, 
        0, NULL
    );

    CloseHandle(hProcess);
    return 0;
}