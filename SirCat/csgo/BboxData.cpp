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
#include <string>
#include <wchar.h>
#include <Windows.h>

using namespace std;

struct BboxData::WinInfo
{
	DWORD dwProcessId;
	DWORD dwThreadId;
	HWND hwnd;
};

struct BboxData::ChildInfo
{
	HWND childHwnd;
	int swExpression;
};

bool BboxData::bUnpackModels(const wstring csgoDir) const
{
	bool bSuccess = false;
	const WCHAR applicationName[] = L".\\HLExtract\\HLExtract.exe";
	WCHAR *commandLine = new WCHAR[32910]; //Sufficiently large to accomodate max extended-length path and command line

	wmemcpy_s(commandLine, 32910, L"HLExtract.exe -p \"", 19);
	wcscat_s(commandLine, 32910, csgoDir.c_str());
	wcscat_s(commandLine, 32910, L"\\csgo\\pak01_dir.vpk\" -d \".\" -e \"root\\models\\player\\custom_player\\legacy\" -s");

	if (bCreateProcess(applicationName, commandLine))
		bSuccess = true;

	delete[] commandLine;
	commandLine = nullptr;

	return bSuccess;
}

bool BboxData::bDecompileModels() const
{
	bool bSuccess = false;
	const WCHAR applicationName[] = L".\\Crowbar\\Crowbar.exe";
	PROCESS_INFORMATION pi;
	WCHAR commandLine[] = L"Crowbar.exe";

	if (bCreateProcess(applicationName, commandLine, &pi, false))
		bSuccess = bWaitForCrowbarWindow(pi);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return bSuccess;
}

bool BboxData::bWaitForCrowbarWindow(const PROCESS_INFORMATION &pi) const
{
	bool bSuccess = false;
	GUITHREADINFO gui;
	WCHAR lpClassName[MAX_PATH];
	WinInfo winInfo;

	winInfo.dwProcessId = pi.dwProcessId;
	winInfo.hwnd = 0;
	BlockInput(TRUE); //No user input so info about GUI elements with focus will be consistent 

	while (GetForegroundWindow() != winInfo.hwnd) //Loop until Crowbar's main window is created
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&winInfo)); //Look for visible owner window using PID+TID

	AttachThreadInput(GetCurrentThreadId(), winInfo.dwThreadId, TRUE);
	gui.cbSize = sizeof(GUITHREADINFO);

	do //Loop allows time for child windows to load and retrieves hwnd of the edit control of interest
	{
		GetGUIThreadInfo(winInfo.dwThreadId, &gui); //Gets the hwnd of the edit control with focus when Crowbar loads
		GetClassNameW(gui.hwndFocus, lpClassName, MAX_PATH);
	} while (wcsstr(lpClassName, L"WindowsForms10.EDIT") == nullptr); //Exit loop when edit control has gained focus

	bSuccess = bAutomateCrowbar(winInfo.hwnd, gui.hwndFocus);
	AttachThreadInput(GetCurrentThreadId(), winInfo.dwThreadId, FALSE);
	SendMessageW(winInfo.hwnd, WM_CLOSE, NULL, NULL); //Tell Crowbar to close
	WaitForSingleObject(pi.hProcess, INFINITE);
	BlockInput(FALSE);

	return bSuccess;
}

bool BboxData::bAutomateCrowbar(const HWND &winInfoHwnd, const HWND &guiHwndFocus) const
{
	bool bSuccess = false;
	DWORD nBufferLength = GetFullPathNameW(L".", 0, nullptr, NULL);

	if (nBufferLength != 0)
	{
		WCHAR *lpBuffer = new WCHAR[nBufferLength + 8];

		if (GetFullPathNameW(L".", nBufferLength, lpBuffer, NULL) == nBufferLength - 1) //Get program's current working directory
		{
			ChildInfo childInfo;
			RECT conRect;
			RECT croRect;

			GetWindowRect(GetConsoleWindow(), &conRect);
			GetWindowRect(winInfoHwnd, &croRect);
			MoveWindow(winInfoHwnd, static_cast<int>(conRect.right), static_cast<int>(conRect.top), //Crowbar blocks the console
				static_cast<int>(croRect.right - croRect.left),	static_cast<int>(croRect.bottom - croRect.top), TRUE);
			wcscat_s(lpBuffer, nBufferLength + 8, L"\\legacy\\"); //Subdirectoy of current working directory w/ HLExtract output
			SetFocus(guiHwndFocus);
			SendMessageW(guiHwndFocus, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(lpBuffer)); //Set 'MDL input:' to subdirectory
			childInfo.childHwnd = guiHwndFocus;
			childInfo.swExpression = 1;
			EnumChildWindows(GetParent(guiHwndFocus), EnumChildProc, reinterpret_cast<LPARAM>(&childInfo)); //Find 'Output to:'
			SetFocus(childInfo.childHwnd);
			SendMessageW(childInfo.childHwnd, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(lpBuffer)); //Set it to subdirectory
			childInfo.swExpression = 2;
			EnumChildWindows(GetParent(guiHwndFocus), EnumChildProc, reinterpret_cast<LPARAM>(&childInfo));
			SendMessageW(childInfo.childHwnd, BM_CLICK, NULL, NULL); //Click the 'Decompile' button

			while (IsWindowEnabled(childInfo.childHwnd) != FALSE); //'Decompile' is disabled when Crowbar begins decompiling

			while (IsWindowEnabled(childInfo.childHwnd) == FALSE); //Wait for 'Decompile' to be re-enabled when it's done

			bSuccess = true;
		}

		delete[] lpBuffer;
		lpBuffer = nullptr;
	}

	return bSuccess;
}

bool BboxData::bReadModelFiles(const bool bCleanLegacyDir)
{
	int success = 0;
	HANDLE hFind;
	WCHAR *lpBuffer = nullptr;
	WCHAR *lpFileName = nullptr;
	WIN32_FIND_DATAW FindFileData;
	DWORD nBufferLength = GetFullPathNameW(L".", 0, nullptr, NULL);

	fetchModelFileDir(static_cast<int>(nBufferLength), hFind, lpBuffer, lpFileName, FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		int i = 0;

		while (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			FindNextFileW(hFind, &FindFileData); //Skip the parent directory result

		while (i < numRows)
		{
			WCHAR *dotFileExt = wcsrchr(FindFileData.cFileName, L'.'); //Locate the dot in the file extension
			wstring absolutePath = wstring(lpFileName) + wstring(FindFileData.cFileName);

			if (wmemcmp(dotFileExt, L".qc", 3) == 0 && !bCleanLegacyDir) //Only parse model files with .qc extension
				success = fetchModelBboxData(i, FindFileData);

			DeleteFileW(absolutePath.c_str());

			if (success == -1 || GetLastError() == ERROR_NO_MORE_FILES)
			{
				success = 0; //Return as unsuccessful if data array is not fully filled after parsing all model files
				break;
			}

			FindNextFileW(hFind, &FindFileData); //Fetch next model file name
		}

		while (GetLastError() != ERROR_NO_MORE_FILES) //Delete remaining files after last model file
		{
			wstring absolutePath;

			FindNextFileW(hFind, &FindFileData);
			absolutePath = wstring(lpFileName) + wstring(FindFileData.cFileName);
			DeleteFileW(absolutePath.c_str());
		}

		FindClose(hFind);
		RemoveDirectoryW(lpFileName);
	}

	if (nBufferLength != 0)
		delete[] lpBuffer;

	return (success > 0); //Return true only if successful (success > 0)
}

bool BboxData::bCreateProcess(const WCHAR *const applicationName, WCHAR *const commandLine,
	PROCESS_INFORMATION *pPi, bool bWaitForExit) const
{
	bool bSuccess;
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
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
	DWORD dwProcessId = 0;
	WinInfo &winInfo = *reinterpret_cast<WinInfo *>(lParam); //lParam cast to WinInfo serves as input & output parameter

	winInfo.dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId); //Use to find Crowbar's main window

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
	ChildInfo &childInfo = *reinterpret_cast<ChildInfo *>(lParam); //lParam cast to childInfo serves as input & output parameter
	WCHAR buffer[MAX_PATH];

	GetClassNameW(hwnd, buffer, MAX_PATH); //Use to narrow down the control type inside the following switch

	switch (childInfo.swExpression)
	{
	case 1:
		if (wcsstr(buffer, L"WindowsForms10.EDIT") != nullptr && childInfo.childHwnd != hwnd) //Skip 'MDL input:' child window
		{
			bContinueEnum = FALSE;
			childInfo.childHwnd = hwnd; //Find the 'Output to:' child window
		}

		break;
	case 2:
		if (wcsstr(buffer, L"WindowsForms10.BUTTON") != nullptr)
		{
			GetWindowTextW(hwnd, buffer, 11); //Other buttons start with "Decompile" but length 11 copies an extra character

			if (wcscmp(buffer, L"Decompile") == 0) //Make sure button's string contains only the string "Decompile"
			{
				bContinueEnum = FALSE;
				childInfo.childHwnd = hwnd; //Find the 'Decompile' button child window
			}
		}
		
		break;
	default:
		break;
	}

	return bContinueEnum;
}

void BboxData::fetchModelFileDir(const int nBufferLength, HANDLE &hFind, WCHAR *&lpBuffer, WCHAR *&lpFileName,
	WIN32_FIND_DATAW &FindFileData) const
{
	if (nBufferLength != 0)
	{
		lpBuffer = new WCHAR[nBufferLength + 13];
		wmemcpy_s(lpBuffer, nBufferLength + 13, L"\\\\?\\", 4); //Prefix to permit extended-length path with later function calls

		if (GetFullPathNameW(L".", nBufferLength, lpBuffer + 4, NULL) == nBufferLength - 1)
		{
			wcscat_s(lpBuffer, nBufferLength + 13, L"\\legacy\\*"); //Add subdirectory where the model files were created
			lpFileName = lpBuffer;
		}
	}

	if (nBufferLength == 0 || lpFileName == nullptr)
		lpFileName = const_cast<WCHAR *>(L".\\legacy\\*"); //Use non-extended-length path if finding extended-length path fails

	hFind = FindFirstFileW(lpFileName, &FindFileData); //Begin fetching model file names

	if (lpFileName == lpBuffer) //First 'file' is the search directory
		lpFileName[nBufferLength + 11] = L'\0'; //Remove * so the path can be prepended to FindFileData.cFileName later
	else
		lpFileName[9] = L'\0'; //For non-extended-length path
}

int BboxData::fetchModelBboxData(int &i, WIN32_FIND_DATAW &FindFileData)
{
	int success = 0;
	WCHAR searchResult[1][MAX_PATH];
	wstring searchTerm = L"$hbox"; //Search term to find the line that contains bbox data
	wifstream modelFile;

	modelFile.open(wstring(L".\\legacy\\") + wstring(FindFileData.cFileName));

	if (modelFile.fail() || textFileOps->parseTextFile(searchTerm, modelFile, searchResult, 1) != 1)
		success = -1; //Failed opening model file or it didn't contain the expected bbox data
	else
	{
		for (int j = 0; j < numColumns; ++j) //Collect each attribute for this model
		{
			int charPos = 1; //Will skip first char (a space) when reading searchResult[0] char by char later
			int elementsToCopy[] = { 2, 3, 4, 5, 6, 7, 11 }; //Only collect bbox size data and not angles
			int spaceDelimitedElement = 0; //Tracks the current space-delimited string element in searchResult[0]
			wstring bboxDatumBuilder;

			do
			{
				if (searchResult[0][charPos] == L' ')
					++spaceDelimitedElement;

				if (spaceDelimitedElement == elementsToCopy[j] && searchResult[0][charPos] != L' ')
					bboxDatumBuilder += searchResult[0][charPos]; //Build bbox datum

				++charPos;

				if (spaceDelimitedElement > elementsToCopy[j] || searchResult[0][charPos] == L'\0')
					break;
			} while (true);

			data[i][j] = bboxDatumBuilder; //Bbox datum is built, add it to bbox data array
		}

		++i;
		success = 1;
	}

	modelFile.close();

	return success;
}
