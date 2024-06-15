// References:
// https://github.com/HaloSPV3/HCE.DRP
// https://github.com/aLTis94/Halo_CE/blob/master/offsets.lua

#include <windows.h>
#include <winerror.h>
#include <psapi.h>
#include <iostream>
#include <fstream>

using namespace std;

HANDLE main_thread;

int main() 
{
    char szProcessName[128];
    GetModuleBaseNameA(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
	MessageBoxA(NULL, "Ola de mar", szProcessName, MB_OK);
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD fwdReason,     
    LPVOID lpvReserved
)
{
	if (fwdReason == DLL_PROCESS_ATTACH && main_thread == NULL) {
		main_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
		if (main_thread == NULL) {
			return false;
		}
	}
	if (fwdReason == DLL_PROCESS_DETACH && main_thread != NULL) {
		TerminateThread(main_thread, 0);
	}
	return true;
}