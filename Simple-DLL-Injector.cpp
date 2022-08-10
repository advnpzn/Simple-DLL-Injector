#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>


DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_strcmpi((char*)procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}

	CloseHandle(hSnap);
	return procId;
}


int main()
{
	const char* dllPath = ""; // Your DLL path.
	const char* procName = ""; // Your executable Path to which you want to inject your DLL into.

	DWORD procId = 0;
	while (!procId) {
		procId = GetProcId(procName);
		Sleep(50);
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

	if (hProc && hProc != INVALID_HANDLE_VALUE)
	{
		void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (loc)
		{
			WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
		}

		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

		if (hThread)
		{
			CloseHandle(hThread);
		}
	}

	if (hProc)
	{
		CloseHandle(hProc);
	}

	return 0;
}