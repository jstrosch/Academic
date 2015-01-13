#pragma comment(lib, "ws2_32.lib")
#define _WIN32_WINNT  0x0601
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>

DWORD CALLBACK Thread_ShellSpawner(LPVOID lpParam)
{

	char szSystemDir[MAX_PATH + 1];

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	//si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW | STARTF_USEPOSITION;
	si.wShowWindow = SW_SHOW;
	//si.hStdError = si.hStdInput = si.hStdOutput = (HANDLE)sClient;
	si.dwX = 0;
	si.dwY = 0;
	
	GetSystemDirectory(szSystemDir, MAX_PATH);

	SetCurrentDirectory(szSystemDir);
	CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);

	return 0;
}

int main(void)
{
	DWORD threadID;
	DWORD threadParam;
	HANDLE hThread = CreateThread(NULL, 0, Thread_ShellSpawner, &threadParam, 0, &threadID);

	if(hThread==NULL)
		puts("Failure");
	else
		puts("Success");

	CloseHandle(hThread);

	return 0;
}