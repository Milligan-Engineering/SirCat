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

FindCsgo &FindCsgo::inst()
{
	static FindCsgo inst;
	return inst;
}

bool FindCsgo::bFetchSteamDir(wstring &steamDir)
{
	HKEY hKey = NULL;
	DWORD keyType = REG_SZ;
	WCHAR keyData[MAX_PATH] = { NULL };
	DWORD keyDataSize = sizeof(keyData);

	LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);

	if (ret == ERROR_SUCCESS) //Check registry for 64-bit Windows installation
	{
		ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &keyType, reinterpret_cast<LPBYTE>(keyData), &keyDataSize);
		RegCloseKey(hKey);
	}

	if (ret == ERROR_FILE_NOT_FOUND) //Then check for 32-bit if unsuccessful
	{
		ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &keyType, reinterpret_cast<LPBYTE>(keyData), &keyDataSize);
			RegCloseKey(hKey);
		}
	}

	if (ret == ERROR_SUCCESS)
	{
		steamDir = static_cast<wstring>(keyData);
		testDir = static_cast<wstring>(keyData) + static_cast<wstring>(L"\\steamapps");
	}

	return !static_cast<bool>(ret); //0 (ERROR_SUCCESS) indicates success, so take logical NOT or ret
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
		if (static_cast<bool>(TextFileOps::inst().parseTextFile(static_cast<wstring>(L"\"installdir\""), manifest, searchResult,
			1, L"\t\"\0", 2)))
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

			if (static_cast<bool>(TextFileOps::inst().parseTextFile(static_cast<wstring>(searchTerm), libFile, searchResult,
				1, L"\t\"\0", 2)))
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
