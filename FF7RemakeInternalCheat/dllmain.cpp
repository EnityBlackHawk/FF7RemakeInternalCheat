#include <Windows.h>
#include "cheat.h"


DWORD WINAPI EndThread(HMODULE hModule)
{
	FreeLibraryAndExitThread(hModule, 0);
}

DWORD WINAPI MainThread(HMODULE hModelue) 
{
	MessageBoxA(NULL, "Welcome", "FFVII", MB_OK | MB_ICONASTERISK);

	AllocConsole();
	SetConsoleTitle("FFVII");
	FILE* f;
	FILE* G;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&G, "CONIN$", "r", stdin);
	
	Cheat::InitCheat();

	FreeConsole();
	fclose(f);
	fclose(G);
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)EndThread, hModelue, 0, nullptr);
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserve)
{

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr));
	}

	return TRUE;

}