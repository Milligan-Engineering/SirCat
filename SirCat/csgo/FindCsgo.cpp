#ifndef STRICT
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "FindCsgo.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

bool FindCsgo::bFetchSteamDir(wstring &steamDir)
{
	HKEY hKey = NULL;
	DWORD type = REG_SZ;
	WCHAR *data = nullptr;
	DWORD cbData = sizeof(WCHAR) * 32767;
	LONG ret;

	bDynamicArrayDeleted = true;
	ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);

	if (ret == ERROR_SUCCESS) //Check registry for 64-bit Windows installation
	{
		newDynamicArray(data);
		ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &type, reinterpret_cast<LPBYTE>(data), &cbData);
		RegCloseKey(hKey);
	}

	if (ret == ERROR_FILE_NOT_FOUND) //Then check for 32-bit if unsuccessful
	{
		ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			if (bDynamicArrayDeleted)
				newDynamicArray(data);

			ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &type, reinterpret_cast<LPBYTE>(data), &cbData);
			RegCloseKey(hKey);
		}
	}

	if (ret == ERROR_SUCCESS)
	{
		steamDir = data;
		testDir = steamDir + static_cast<wstring>(L"\\steamapps");
	}

	if (!bDynamicArrayDeleted)
		deleteDynamicArray(data);

	return ret == ERROR_SUCCESS;
}

bool FindCsgo::bCheckCsgoInstall()
{
	bool bFoundCsgoInstall = false;
	wifstream manifest;

	manifest.open(testDir + static_cast<wstring>(L"\\appmanifest_730.acf"));

	if (!manifest.fail())
	{
		WCHAR searchResult[1][MAX_PATH];

		//Verify CS:GO installation directory listed in manifest file contents
		if (textFileOps.parseTextFile(static_cast<wstring>(L"\"installdir\""), manifest, searchResult, 1, L"\t\"\0", 2) != 0)
		{
			bFoundCsgoInstall = true;
			testDir += static_cast<wstring>(L"\\common\\") + static_cast<wstring>(searchResult[0]);
		}

		manifest.close();
	}

	return bFoundCsgoInstall;
}

bool FindCsgo::bSearchSteamLibs()
{
	bool bFoundCsgoInstall = false;
	wifstream libFile;

	libFile.open(testDir + static_cast<wstring>(L"\\libraryfolders.vdf"));

	if (!libFile.fail())
	{
		for (int i = 1; i < 10; i++)
		{
			WCHAR searchTerm[] = { L'\"', static_cast<WCHAR>(i + static_cast<int>(L'0')), L'\"', L'\0' };
			WCHAR searchResult[1][MAX_PATH];

			if (textFileOps.parseTextFile(static_cast<wstring>(searchTerm), libFile, searchResult, 1, L"\t\"\0", 2) != 0)
			{
				testDir = static_cast<wstring>(searchResult[0]) + static_cast<wstring>(L"\\steamapps");

				if (bCheckCsgoInstall())
				{
					bFoundCsgoInstall = true;
					break;
				}
			}
			else
				break;
		}

		libFile.close();
	}

	return bFoundCsgoInstall;
}

wstring FindCsgo::getTestDir()
{
	return testDir;
}

void FindCsgo::newDynamicArray(WCHAR *&data)
{
	bDynamicArrayDeleted = false;
	data = new WCHAR[32767];

	for (int i = 0; i < 32767; ++i)
		data[i] = '\0';
}

void FindCsgo::deleteDynamicArray(WCHAR *&data)
{
	delete[] data;
	data = nullptr;
	bDynamicArrayDeleted = true;
}
