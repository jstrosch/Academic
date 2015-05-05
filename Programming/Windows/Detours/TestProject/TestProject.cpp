// TestProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	DeleteFileA("C:\\tmp\\SAMPLE.txt");

	return 0;
}

