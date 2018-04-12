#ifndef STRICT
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "BboxData.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>
#include <shellapi.h>
#include <Windows.h>

using namespace std;

bool BboxData::bArchiveObjMade = false;
wstring BboxData::modelNames[] = { L"" };
wstring BboxData::attrNames[] = { L"" };

BboxData::BboxData()
{
	numColumns = k_num_attr;
}

bool BboxData::bMakeBboxObjArchive(const wstring csvName)
{
	bool bSuccess = false;

	if (!bArchiveObjMade) //Only one archive can be made because this gets set to true inside
	{
		//Parameters to pass for slicing out headers that are member arrays
		wstring *headers[2] = { modelNames, attrNames };
		const int sliceSize[2] = { k_num_model, k_num_attr };
		const bool sliceIsRow[2] = { false, true };
		const int numSlice[2] = { 1, 1 };

		Archive::csvName = csvName;

		if (bMakeObjArchive(2, headers, sliceSize, sliceIsRow, numSlice))
		{
			bArchiveObjMade = true;
			bSuccess = true;
		}
	}

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

	wstring applicationNameWS = L".\\Crowbar\\Crowbar.exe";
	wstring commandLineWS = L"Crowbar.exe";

	return bCreateProcess(applicationNameWS, commandLineWS);
}

bool BboxData::bReadModelFiles()
{
	bool bSuccess = false;
	HANDLE hFind;
	WIN32_FIND_DATAW FindFileData;

	hFind = FindFirstFileW(static_cast<LPCWSTR>(L".\\legacy\\*.qc"), &FindFileData); //Begin fetching model file names

	if (hFind != INVALID_HANDLE_VALUE)
	{
		for (int i = 0; i < k_num_model; ++i) //Collect attributes for each model in bboxData
		{
			size_t dotIndex = static_cast<wstring>(FindFileData.cFileName).find_last_of(L"."); //Where to trim file extension

			if (static_cast<wstring>(FindFileData.cFileName).substr(static_cast<size_t>(0), dotIndex) != modelNames[i]
				&& !static_cast<bool>(hFind) && i < k_num_model - 1 && GetLastError() != ERROR_NO_MORE_FILES)
			{
				bSuccess = false; //Model filename did not match expected modelNames[i] or FindNextFileW failed early
				break;
			}
			else
			{
				wifstream modelFile;

				modelFile.open(static_cast<wstring>(L".\\legacy\\") + static_cast<wstring>(FindFileData.cFileName));

				if (!modelFile.fail())
				{
					WCHAR searchResult[1][MAX_PATH];
					wstring searchTerm = L"$hbox";
					int elementsToCopy[k_num_attr] = { 2, 3, 4, 5, 6, 7, 11 };
					int charPos = 1; //Will skip first char (a space) when reading searchResult[0] char by char later
					int spaceDelimitedElement = 0; //Tracks the current space-delimited string element in searchResult[0]

					if (textFileOps.parseTextFile(searchTerm, modelFile, searchResult, 1) != 1)
					{
						bSuccess = false;
						break;
					}

					for (int j = 0; j < k_num_attr; ++j) //Collect each attribute for this model
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

						bboxData[i][j] = bboxDatumBuilder;
					}

					bSuccess = true;
					FindNextFileW(hFind, &FindFileData); //Fetch next model file name
					modelFile.close();
				}
			}
		}

		FindClose(hFind);
	}

	deleteLegacyDir();

	return bSuccess;
}

bool BboxData::bCheckArchive(BboxData &newBbox, wstring &badRowName, wstring &badColName, wstring &badNewVal,
	wstring &badArchiveVal)
{
	bool bUpdate = false;
	int j;

	for (int i = 0; i < k_num_model; ++i)
	{
		if (bUpdate = bCheckArchiveRow(newBbox.bboxData[i], bboxData[i], j)) //Single = is intentional
		{
			badRowName = modelNames[i];
			badColName = attrNames[j];
			badNewVal = newBbox.bboxData[i][j];
			badArchiveVal = bboxData[i][j];
			break; //Terminate the loop after first mismatch
		}
	}

	return bUpdate;
}

void BboxData::readArchive()
{
	for (int i = 0; i < k_num_model; ++i)
		readArchiveRow(bboxData[i], i + 2);
}

bool BboxData::bWriteArchiveFile(BboxData &newBbox)
{
	bool bSuccess = false;

	getOutArchive().open(csvName);

	if (!getOutArchive().fail())
	{
		writeArchiveFileRow(attrNames);
		getOutArchive() << endl;

		for (int i = 0; i < k_num_model; ++i)
		{
			getOutArchive() << modelNames[i];
			writeArchiveFileRow(newBbox.bboxData[i]);
			getOutArchive() << endl;
		}

		bSuccess = true;
		getOutArchive().close();
	}

	return bSuccess;
}

bool BboxData::bCreateProcess(wstring applicationNameWS, wstring commandLineWS)
{
	bool bSuccess;
	WCHAR *applicationName = &applicationNameWS[0];
	WCHAR *commandLine = &commandLineWS[0];
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	bSuccess = CreateProcessW(applicationName, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) != 0;
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return bSuccess;
}

void BboxData::deleteLegacyDir()
{
	TCHAR dir[MAX_PATH + 1] = L"";
	SHFILEOPSTRUCTW fileOp;
	int len = static_cast<int>(GetCurrentDirectoryW(MAX_PATH, dir));

	if (len + 8 <= MAX_PATH)
	{
		WCHAR legacySubDir[9] = L"\\legacy";

		for (int i = 0; i < 9; ++i)
			dir[len + i] = legacySubDir[i];

		fileOp.hwnd = NULL;
		fileOp.wFunc = FO_DELETE;
		fileOp.pFrom = dir;
		fileOp.pTo = NULL;
		fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
		fileOp.fAnyOperationsAborted = FALSE;
		fileOp.hNameMappings = NULL;
		fileOp.lpszProgressTitle = NULL;
		SHFileOperationW(&fileOp);
	}
}
