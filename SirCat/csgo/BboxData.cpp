#ifndef __STDC_WANT_LIB_EXT1__
	#define __STDC_WANT_LIB_EXT1__ 1
#endif //__STDC_WANT_LIB_EXT1__

#ifndef STRICT
	#define STRICT 1
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
#endif //WIN32_LEAN_AND_MEAN

#include "BboxData.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <iostream> ////////////////////////////////////////////////////
#include <string>
#include <wchar.h>
#include <Windows.h>

using namespace std;

struct BboxData::WinInfo
{
	DWORD dwProcessId;
	HWND hwnd;
	DWORD dwThreadId;
};

bool BboxData::bUnpackModels(const wstring csgoDir) const
{
	//******Check for existence of .\HLExtract\HLExtract.exe and .\HLExtract\HLLib.dll******
	//******add comments******

	bool bSuccess = false;
	const WCHAR applicationName[] = L".\\HLExtract\\HLExtract.exe";
	WCHAR *commandLine = new WCHAR[32910];

	wmemcpy_s(commandLine, 32910, L"HLExtract.exe -p \"", 19);
	wcscat_s(commandLine, 32910, csgoDir.c_str());
	wcscat_s(commandLine, 32910, L"\\csgo\\pak01_dir.vpk\" -d \".\" -e \"root\\models\\player\\custom_player\\legacy\" -s");
	//WCHAR *commandLine = static_cast<wstring>(L"HLExtract.exe -p \"") + csgoDir
		//+ static_cast<wstring>(L"\\csgo\\pak01_dir.vpk\" -d \".\" -e \"root\\models\\player\\custom_player\\legacy\" -s");

	if (bCreateProcess(applicationName, commandLine))
		bSuccess = true;

	delete[] commandLine;
	commandLine = nullptr;

	return bSuccess;
}

bool BboxData::bDecompileModels() const
{
	//******Check for existence of .\Crowbar\Crowbar.exe******
	//******Write requisite entries for Crowbar Settings.xml******
	//******Use child windows in Decompile tab to set options?******
	//******add comments******

	bool bSuccess = false;
	const WCHAR applicationName[] = L".\\Crowbar\\Crowbar.exe";
	WCHAR commandLine[] = L"Crowbar.exe";
	PROCESS_INFORMATION pi;

	if (bCreateProcess(applicationName, commandLine, &pi, false))
	{
			GUITHREADINFO gui;
			WCHAR lpClassName[MAX_PATH];
			DWORD nBufferLength;
			WinInfo winInfo;

			winInfo.dwProcessId = pi.dwProcessId;
			winInfo.hwnd = 0;
			BlockInput(TRUE);

			while (GetForegroundWindow() != winInfo.hwnd)
				EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&winInfo));

			AttachThreadInput(GetCurrentThreadId(), winInfo.dwThreadId, TRUE);
			gui.cbSize = sizeof(GUITHREADINFO);

			do
			{
				GetGUIThreadInfo(winInfo.dwThreadId, &gui);
				GetClassNameW(gui.hwndFocus, lpClassName, MAX_PATH);
			} while (wcsstr(lpClassName, L"WindowsForms10.EDIT") == nullptr);

			nBufferLength = GetFullPathNameW(L".", 0, nullptr, NULL);

			if (nBufferLength != 0)
			{
				WCHAR *lpBuffer = new WCHAR[nBufferLength + 8];

				if (GetFullPathNameW(L".", nBufferLength, lpBuffer, NULL) == nBufferLength - 1)
				{
					wcscat_s(lpBuffer, nBufferLength + 8, L"\\legacy\\");

					if (SendMessageW(gui.hwndFocus, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(lpBuffer)) == TRUE)
					{
						HWND childHwnd = gui.hwndFocus;
						EnumChildWindows(GetParent(gui.hwndFocus), EnumChildProc, reinterpret_cast<LPARAM>(&childHwnd));
						SetFocus(childHwnd);

						if (SendMessageW(childHwnd, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(lpBuffer)) == TRUE)
							bSuccess = true;
					}
				}

				delete[] lpBuffer;
				lpBuffer = nullptr;
			}

			AttachThreadInput(GetCurrentThreadId(), winInfo.dwThreadId, FALSE);
			BlockInput(FALSE); //Move after WaitForSingleObject after closing it is automated
			WaitForSingleObject(pi.hProcess, INFINITE);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return bSuccess;
}

bool BboxData::bReadModelFiles()
{
	bool bSuccess = false;
	WCHAR *lpBuffer = nullptr;
	WCHAR *lpFileName = nullptr;
	HANDLE hFind;
	WIN32_FIND_DATAW FindFileData;
	DWORD nBufferLength = GetFullPathNameW(L".", 0, nullptr, NULL);

	//******Check do-while loop for correct handling of ERROR_NO_MORE_FILES and other unexpected circumstances******

	if (nBufferLength != 0)
	{
		lpBuffer = new WCHAR[nBufferLength + 13];
		wmemcpy_s(lpBuffer, nBufferLength + 13, L"\\\\?\\", 4); //Prefix to permit extended-length path with later function calls

		if (GetFullPathNameW(L".", nBufferLength, lpBuffer + 4, NULL) == nBufferLength - 1)
		{
			wcscat_s(lpBuffer, nBufferLength + 13, L"\\legacy\\*");
			lpFileName = lpBuffer;
		}
	}

	if (nBufferLength == 0 || lpFileName == nullptr)
		lpFileName = const_cast<WCHAR *>(L".\\legacy\\*");

	hFind = FindFirstFileW(lpFileName, &FindFileData); //Begin fetching model file names

	if (lpFileName == lpBuffer)
		lpFileName[nBufferLength + 11] = L'\0'; //Remove * so the path can be prepended to FindFileData.cFileName later
	else
		lpFileName[9] = L'\0'; //For non-extended-length path

	if (hFind != INVALID_HANDLE_VALUE)
	{
		int i = 0;

		while (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			FindNextFileW(hFind, &FindFileData); //Skip the parent directory result

		do //Collect attributes for each model in bboxData
		{
			wstring absolutePath = static_cast<wstring>(lpFileName) + static_cast<wstring>(FindFileData.cFileName);
			WCHAR *dotFileExt = wcsrchr(FindFileData.cFileName, L'.'); //Locate the dot in the file extension

			//if (i < k_num_model - 1 && GetLastError() == ERROR_NO_MORE_FILES)
			//{
				//bSuccess = false; //Model filename did not match expected modelNames[i]
				//DeleteFileW(absolutePath.c_str());
				//break;
			//}

			if (wmemcmp(dotFileExt, L".qc", 3) == 0) //Check if file extension is qc
			{
				wstring searchTerm = L"$hbox";
				WCHAR searchResult[1][MAX_PATH];
				wifstream modelFile;

				modelFile.open(static_cast<wstring>(L".\\legacy\\") + static_cast<wstring>(FindFileData.cFileName));

				if (modelFile.fail() || textFileOps->parseTextFile(searchTerm, modelFile, searchResult, 1) != 1)
				{
					bSuccess = false;
					modelFile.close();
					DeleteFileW(absolutePath.c_str());
					break;
				}

				for (int j = 0; j < numColumns; ++j) //Collect each attribute for this model
				{
					int charPos = 1; //Will skip first char (a space) when reading searchResult[0] char by char later
					int spaceDelimitedElement = 0; //Tracks the current space-delimited string element in searchResult[0]
					wstring bboxDatumBuilder;
					int elementsToCopy[] = { 2, 3, 4, 5, 6, 7, 11 };

					do
					{
						if (searchResult[0][charPos] == L' ')
							++spaceDelimitedElement;

						if (spaceDelimitedElement == elementsToCopy[j] && searchResult[0][charPos] != L' ')
							bboxDatumBuilder += searchResult[0][charPos]; //Build bbox datum at the element of interest

						++charPos;

						if (spaceDelimitedElement > elementsToCopy[j] || searchResult[0][charPos] == L'\0')
							break;
					} while (true);

					data[i][j] = bboxDatumBuilder;
				}

				++i;
				bSuccess = true;
				modelFile.close();
			}

			DeleteFileW(absolutePath.c_str());
			FindNextFileW(hFind, &FindFileData); //Fetch next model file name
		} while (i < numRows);

		while (GetLastError() != ERROR_NO_MORE_FILES) //Delete remaining files after last model file
		{
			wstring absolutePath;

			FindNextFileW(hFind, &FindFileData);
			absolutePath = static_cast<wstring>(lpFileName) + static_cast<wstring>(FindFileData.cFileName);
			DeleteFileW(absolutePath.c_str());
		}

		FindClose(hFind);
		RemoveDirectoryW(lpFileName);
	}

	if (nBufferLength != 0)
		delete[] lpBuffer;

	lpBuffer = nullptr;
	lpFileName = nullptr;

	return bSuccess;
}

bool BboxData::bCreateProcess(const WCHAR *const applicationName, WCHAR *const commandLine,
	PROCESS_INFORMATION *pPi, bool bWaitForExit) const
{
	bool bSuccess;
	//WCHAR *applicationName = &applicationNameWS[0];
	//WCHAR *commandLine = &commandLineWS[0];
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	bSuccess = CreateProcessW(applicationName, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) != 0;

	if (pPi != nullptr)
		*pPi = pi;
	else
	{
		if (bWaitForExit)
			WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return bSuccess;
}

BOOL BboxData::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	BOOL bContinueEnum = TRUE;
	WinInfo &winInfo = *reinterpret_cast<WinInfo *>(lParam);
	DWORD dwProcessId = 0;

	winInfo.dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);

	if (winInfo.dwProcessId == dwProcessId && GetWindow(hwnd, GW_OWNER) == NULL && IsWindowVisible(hwnd) == TRUE)
	{
		bContinueEnum = FALSE;
		winInfo.hwnd = hwnd;
	}

	return bContinueEnum;
}

BOOL BboxData::EnumChildProc(HWND hwnd, LPARAM lParam)
{
	BOOL bContinueEnum = TRUE;	
	HWND &childHwnd = *reinterpret_cast<HWND *>(lParam);
	WCHAR buffer[MAX_PATH];

	GetClassNameW(hwnd, buffer, MAX_PATH);

	if (wcsstr(buffer, L"WindowsForms10.EDIT") != nullptr && childHwnd != hwnd)
	{
		bContinueEnum = FALSE;
		childHwnd = hwnd; //Found the other WindowsForms10.EDIT control, so send its hwnd back with LPARAM and stop enumerating
	}

	return bContinueEnum;
}
