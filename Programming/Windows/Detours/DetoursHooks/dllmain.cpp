
#include "stdafx.h"

#include "windows.h"
#include "stdio.h"
#include "..\shared\detours.h"
#include <ws2tcpip.h>

#pragma comment (lib, "..\\shared\\detours.lib")
#pragma comment (lib, "ws2_32.lib")

// Create pointers to the original functions
static BOOL(WINAPI *RealDeleteFileA) (LPCSTR) = DeleteFileA;
int (WINAPI *RealConnect)(SOCKET, const struct sockaddr*, int) = connect;
int (WINAPI *RealWSAConnect)(SOCKET, const struct sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS) = WSAConnect;

int WINAPI HookWSAConnect(SOCKET s, const struct sockaddr *name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS){
	SOCKADDR_IN* name_in = (SOCKADDR_IN*)name;

	FILE *file;
	fopen_s(&file, "C:\\tmp\\wsaconnect.txt", "a+");
	fprintf(file, "[DetourHook] Networking Activity...IP: %s - Port: %d\n", inet_ntoa( name_in->sin_addr), ntohs( name_in->sin_port));
	fclose(file);
	
	return RealWSAConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS);
}

static BOOL(WINAPI *RealCreateProcessW)(
	LPCWSTR, LPWSTR,
	LPSECURITY_ATTRIBUTES,
	LPSECURITY_ATTRIBUTES,
	BOOL, DWORD, LPVOID, LPCWSTR,
	LPSTARTUPINFOW,
	LPPROCESS_INFORMATION) = CreateProcessW;

BOOL WINAPI HookCreateProcessW(LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	)
{
	DWORD dwLastError = GetLastError();
	BOOL  bResult = FALSE;
	CHAR  szDetouredDll[MAX_PATH];
	CHAR  szDllName[MAX_PATH];
	HMODULE hMod1 = NULL, hMod2 = NULL;

	// get the full path to the hooking DLL
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)&HookCreateProcessW,
		&hMod2);

	GetModuleFileNameA(hMod2, szDllName, MAX_PATH);

	OutputDebugStringA(szDllName);
	OutputDebugStringA("\n");

	wprintf(L"[DetoursHooks] Intercepting the creation of %s\n", lpCommandLine);

	// route creation of new process through 
	// the detours API 
	bResult = DetourCreateProcessWithDll(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		szDllName,
		(PDETOUR_CREATE_PROCESS_ROUTINEW)RealCreateProcessW);

	SetLastError(dwLastError);
	return bResult;
}

int WINAPI HookConnect(SOCKET s, const struct sockaddr *name, int namelen)
{

	SOCKADDR_IN* name_in = (SOCKADDR_IN*)name;

	printf("[DetourHook] Networking Activity... %s on port %s\n", inet_ntoa(name_in->sin_addr), ntohs(name_in->sin_port));

	//call original function
	return RealConnect(s, name, namelen);
}

// detours function 
BOOL WINAPI HookDeleteFileA(LPCSTR lpFileName)
{
	// save the last error
	DWORD dwLastError = GetLastError();

	// check if the parameter is valid
	if (lpFileName != NULL && strrchr(lpFileName, '\\') != NULL)
	{
		// allocate memory for copied file name
		PCHAR lpNewFile = new CHAR[MAX_PATH * 2];
		if (lpNewFile != NULL)
		{
			// make sure you have a path called c:\archve
			sprintf_s(lpNewFile,
				MAX_PATH,
				"c:\\tmp\\archive\\%s",
				strrchr(lpFileName, '\\') + 1);
			// copy the file to be deleted into an archive 
			printf("[DetoursHooks] Copying %s => %s\n", lpFileName, lpNewFile);
			CopyFileA(lpFileName, lpNewFile, FALSE);
			delete[] lpNewFile;
		}
	}

	// restore last error
	SetLastError(dwLastError);
	return RealDeleteFileA(lpFileName);
}


extern "C" __declspec(dllexport) void DummyFunc(void)
{
	return;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD   dwReason,
	LPVOID  lpReserved
	)
{
	// install the hook(s)
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)RealDeleteFileA, HookDeleteFileA);
		if (DetourTransactionCommit() == NO_ERROR)
			printf("%s\n", "[Detour] DeleteFileA successfully detoured...");

		//DetourAttach(&(PVOID&)RealCreateProcessW, HookCreateProcessW);
		//DetourAttach(&(PVOID&)RealConnect, HookConnect);
		//DetourAttach(&(PVOID&)RealWSAConnect, HookWSAConnect);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)RealWSAConnect, HookWSAConnect);
		if (DetourTransactionCommit() == NO_ERROR)
			puts("[Detour] WSAConnect successfully detoured...");
		else
			puts("[Detour] WSAConnect failed...");

		//DetourTransactionCommit();
	}
	// uninstall the hook(s)
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)RealDeleteFileA, HookDeleteFileA);
		//DetourDetach(&(PVOID&)RealCreateProcessW, HookCreateProcessW);
		//DetourDetach(&(PVOID&)RealConnect, HookConnect);
		DetourDetach(&(PVOID&)RealWSAConnect, HookWSAConnect);
		DetourTransactionCommit();
	}
	return TRUE;
}
