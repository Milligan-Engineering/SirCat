#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "FindCsgo.h"
#include "TextFileOps.h"
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
		WCHAR blank[] = L"";
		WCHAR steamappsFolder[] = L"\\steamapps";

		steamDir = static_cast<wstring>(keyData);
		TextFileOps::inst().concatCharArrays(keyData, blank, testDir, MAX_PATH);
		TextFileOps::inst().concatCharArrays(testDir, steamappsFolder, testDir, MAX_PATH);
	}

	return !static_cast<bool>(ret); //0 (ERROR_SUCCESS) indicates success, so take logical NOT or ret
}

bool FindCsgo::bCheckCsgoInstall()
{
	bool bFoundCsgoInstall = false;
	wifstream manifest;

	manifest.open(static_cast<wstring>(testDir) + static_cast<wstring>(L"\\appmanifest_730.acf"));

	if (!manifest.fail())
	{
		WCHAR searchResult[1][MAX_PATH];

		//Verify CS:GO installation directory listed in manifest file contents
		if (static_cast<bool>(TextFileOps::inst().parseTextFile(static_cast<wstring>(L"\"installdir\""), manifest, searchResult,
			1, L"\t\"\0", 2)))
		{
			bFoundCsgoInstall = true;
			WCHAR installSubDir[] = L"\\common\\";

			TextFileOps::inst().concatCharArrays(testDir, installSubDir, testDir, MAX_PATH);
			TextFileOps::inst().concatCharArrays(testDir, searchResult[0], testDir, MAX_PATH);
		}

		manifest.close();
	}

	return bFoundCsgoInstall;
}

bool FindCsgo::bSearchSteamLibs()
{
	bool bFoundCsgoInstall = false;
	wifstream libFile;

	libFile.open(static_cast<wstring>(testDir) + static_cast<wstring>(L"\\libraryfolders.vdf"));

	if (!libFile.fail())
	{
		for (int i = 1; i < 10; i++)
		{
			WCHAR searchTerm[] = { L'\"', intDigitToWchar(i), L'\"', L'\0' };
			WCHAR searchResult[1][MAX_PATH];

			if (static_cast<bool>(TextFileOps::inst().parseTextFile(static_cast<wstring>(searchTerm), libFile, searchResult,
				1, L"\t\"\0", 2)))
			{
				WCHAR steamappsFolder[] = L"\\steamapps";

				TextFileOps::inst().concatCharArrays(searchResult[0], steamappsFolder, testDir, MAX_PATH);

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

WCHAR FindCsgo::intDigitToWchar(const int intDigit)
{
	return (static_cast<WCHAR>(intDigit + static_cast<int>(L'0')));
}

wstring FindCsgo::getTestDir()
{
	return static_cast<wstring>(testDir);
}
