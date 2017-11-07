#include "hooks.h"
#include "utilities.h"

HANDLE Utilities::curProcess = (HANDLE)0xFFFFFFFFFFFFFFFF;
HINSTANCE Utilities::hAppInstance = 0;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		Utilities::curProcess = GetCurrentProcess();
		Utilities::hAppInstance = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Hooks::Initialize, NULL, NULL, NULL);
		break;
	}
	return TRUE;
}