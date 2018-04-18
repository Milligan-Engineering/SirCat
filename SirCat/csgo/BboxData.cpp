#ifndef STRICT
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "BboxData.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <iostream> ////////////////////////////////////////////////////
#include <string>
#include <shellapi.h>
#include <Windows.h>

using namespace std;

struct WinInfo
{
	unsigned long dwProcessId;
	HWND hwnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

BboxData::BboxData()
{
}

bool BboxData::bMakeBboxObjArchive(const wstring csvName)
{
	bool bSuccess = false;

	numRows = textFileOps.fetchNumRows(getInArchive(), csvName) - 1;
	numColumns = textFileOps.fetchNumColumns(getInArchive(), csvName) - 1;
	rowHeaders = new wstring[numRows];
	columnHeaders = new wstring[numColumns];
	Archive::csvName = csvName;

	//Parameters to pass for slicing out headers
	wstring *headers[2] = { rowHeaders, columnHeaders };
	const int sliceSize[2] = { numRows, numColumns };
	const bool sliceIsRow[2] = { false, true };
	const int numSlice[2] = { 1, 1 };

	if (bMakeObjArchive(2, headers, sliceSize, sliceIsRow, numSlice))
		bSuccess = true;

	return bSuccess;
}

bool BboxData::bUnpackModels(const wstring csgoDir)
{
	//******Check for existence of .\HLExtract\HLExtract.exe and .\HLExtract\HLLib.dll******

	wstring applicationNameWS = L".\\HLExtract\\HLExtract.exe";
	wstring commandLineWS = static_cast<wstring>(L"HLExtract.exe -p \"") + csgoDir
		+ static_cast<wstring>(L"\\csgo\\pak01_dir.vpk\" -d \".\" -e \"root\\models\\player\\custom_player\\legacy\" -s");

	return bCreateProcess(applicationNameWS, commandLineWS);
}

bool BboxData::bDecompileModels()
{
	//******Check for existence of .\Crowbar\Crowbar.exe******

	bool bSuccess;
	wstring applicationNameWS = L".\\Crowbar\\Crowbar.exe";
	wstring commandLineWS = L"Crowbar.exe";
	unsigned long dwProcessId = 0;

	if (bSuccess = bCreateProcess(applicationNameWS, commandLineWS, &dwProcessId, CREATE_NEW_PROCESS_GROUP)) //Single = is intentional
	{
		WinInfo winInfo;

		winInfo.dwProcessId = dwProcessId;
		winInfo.hwnd = 0;
		EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&winInfo));
		wcout << winInfo.hwnd << endl << endl; ////////////////////////////////////////////////////////////////////////////
	}

	return bSuccess;
}

bool BboxData::bReadModelFiles()
{
	bool bSuccess = false;
	HANDLE hFind;
	WCHAR *lpFileName = nullptr;
	WCHAR *lpBuffer = nullptr;
	WIN32_FIND_DATAW FindFileData;
	DWORD nBufferLength = GetFullPathNameW(static_cast<LPCWSTR>(L"."), 0, nullptr, NULL);

	numRows = 74; ///////////////////////////////////////////////////////////////////////////////////////////////
	numColumns = 7; ////////////////////////////////////////////////////////////////////////////////
	data = new wstring*[numRows]; ////////////////////////////////////////////////////////////////////

	for (int i = 0; i < numRows; ++i) ////////////////////////////////////////////////////////////////
		data[i] = new wstring[numColumns]; //////////////////////////////////////////////////////////////

	if (nBufferLength != 0)
	{
		WCHAR legacySubDir[] = L"\\legacy\\*";
		WCHAR prefix[] = L"\\\\?\\";

		lpBuffer = new WCHAR[nBufferLength + 13];

		for (int i = 0; i < 4; ++i)
			lpBuffer[i] = prefix[i];

		if (GetFullPathNameW(static_cast<LPCWSTR>(L"."), nBufferLength, lpBuffer + 4, NULL) == nBufferLength - 1)
		{
			for (int i = 0; i < 10; ++i)
				lpBuffer[nBufferLength + i + 3] = legacySubDir[i];

			lpFileName = lpBuffer;
		}
	}

	if (nBufferLength == 0 || lpFileName == nullptr)
	{
		WCHAR legacyRelDir[] = L".\\legacy\\*"; //Fall back to non-extended-length paths when extended-path functions fail
		lpFileName = legacyRelDir;
	}

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
			size_t dotIndex = static_cast<wstring>(FindFileData.cFileName).find_last_of(L'.'); //Where to trim file extension
			wstring absolutePath = static_cast<wstring>(lpFileName) + static_cast<wstring>(FindFileData.cFileName);

			//if (i < k_num_model - 1 && GetLastError() == ERROR_NO_MORE_FILES)
			//{
				//bSuccess = false; //Model filename did not match expected modelNames[i]
				//DeleteFileW(static_cast<LPCWSTR>(absolutePath.c_str()));
				//break;
			//}
			if (static_cast<wstring>(FindFileData.cFileName).substr(dotIndex) == L".qc")
			{
				wifstream modelFile;
				WCHAR searchResult[1][MAX_PATH];
				wstring searchTerm = L"$hbox";
				int elementsToCopy[] = { 2, 3, 4, 5, 6, 7, 11 };
				int charPos = 1; //Will skip first char (a space) when reading searchResult[0] char by char later
				int spaceDelimitedElement = 0; //Tracks the current space-delimited string element in searchResult[0]

				modelFile.open(static_cast<wstring>(L".\\legacy\\") + static_cast<wstring>(FindFileData.cFileName));

				if (modelFile.fail() || textFileOps.parseTextFile(searchTerm, modelFile, searchResult, 1) != 1)
				{
					bSuccess = false;
					modelFile.close();
					DeleteFileW(static_cast<LPCWSTR>(absolutePath.c_str()));
					break;
				}

				for (int j = 0; j < numColumns; ++j) //Collect each attribute for this model
				{
					wstring bboxDatumBuilder;

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

			DeleteFileW(static_cast<LPCWSTR>(absolutePath.c_str()));
			FindNextFileW(hFind, &FindFileData); //Fetch next model file name
		} while (i < numRows);

		while (GetLastError() != ERROR_NO_MORE_FILES) //Delete remaining files after last model file
		{
			wstring absolutePath;

			FindNextFileW(hFind, &FindFileData);
			absolutePath = static_cast<wstring>(lpFileName) + static_cast<wstring>(FindFileData.cFileName);
			DeleteFileW(static_cast<LPCWSTR>(absolutePath.c_str()));
		}

		FindClose(hFind);
		RemoveDirectoryW(static_cast<LPCWSTR>(lpFileName));
	}

	if (nBufferLength != 0)
		delete[] lpBuffer;

	lpBuffer = nullptr;
	lpFileName = nullptr;

	return bSuccess;
}

bool BboxData::bCheckArchive(BboxData &newBbox, wstring &badRowName, wstring &badColName, wstring &badNewVal,
	wstring &badArchiveVal)
{
	bool bUpdate = false;
	int j;

	for (int i = 0; i < numRows; ++i)
	{
		if (bUpdate = bCheckArchiveRow(newBbox.data[i], data[i], j)) //Single = is intentional
		{
			badRowName = rowHeaders[i];
			badColName = columnHeaders[j];
			badNewVal = newBbox.data[i][j];
			badArchiveVal = data[i][j];
			break; //Terminate the loop after first mismatch
		}
	}

	return bUpdate;
}

void BboxData::readArchive()
{
	data = new wstring*[numRows];

	for (int i = 0; i < numRows; ++i)
	{
		data[i] = new wstring[numColumns];
		readArchiveRow(data[i], i + 2);
	}
}

bool BboxData::bWriteArchiveFile(BboxData &newBbox)
{
	bool bSuccess = false;

	getOutArchive().open(csvName);

	if (!getOutArchive().fail())
	{
		writeArchiveFileRow(columnHeaders);
		getOutArchive() << endl;

		for (int i = 0; i < numRows; ++i)
		{
			getOutArchive() << rowHeaders[i];
			writeArchiveFileRow(newBbox.data[i]);
			getOutArchive() << endl;
		}

		bSuccess = true;
		getOutArchive().close();
	}

	return bSuccess;
}

bool BboxData::bCreateProcess(wstring applicationNameWS, wstring commandLineWS,
	unsigned long *pDwProcessId, unsigned long dwCreationFlags)
{
	bool bSuccess;
	WCHAR *applicationName = &applicationNameWS[0];
	WCHAR *commandLine = &commandLineWS[0];
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	bSuccess = CreateProcessW(applicationName, commandLine, NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &si, &pi) != 0;

	if (pDwProcessId != nullptr)
		*pDwProcessId = pi.dwProcessId;

	//if (dwCreationFlags == 0) //////////////////////////////////////////////////////////////////////////
		WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return bSuccess;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	BOOL bContinueEnum = TRUE;
	WinInfo &winInfo = *reinterpret_cast<WinInfo*>(lParam);
	unsigned long dwProcessId = 0;

	GetWindowThreadProcessId(hwnd, &dwProcessId);

	if (winInfo.dwProcessId == dwProcessId && GetWindow(hwnd, GW_OWNER) == NULL && IsWindowVisible(hwnd) == TRUE)
	{
		bContinueEnum = FALSE;
		winInfo.hwnd = hwnd;
	}

	return bContinueEnum;
}
