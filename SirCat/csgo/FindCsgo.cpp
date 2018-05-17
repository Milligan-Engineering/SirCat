#ifndef __STDC_WANT_LIB_EXT1__
	#define __STDC_WANT_LIB_EXT1__ 1
#endif //__STDC_WANT_LIB_EXT1__

#include "FindCsgo.h"
#include "..\targetver.h"
#include "..\util\TextFileOps.h"
#include <cwchar>
#include <fstream>
#include <string>
#include <Windows.h>
#include <PathCch.h>

namespace sircat {
namespace csgo {

using sircat::util::TextFileOps;
using std::wifstream;
using std::wstring;

bool FindCsgo::bFetchSteamDir(wstring &steamDir)
{
	DWORD cbData = sizeof(WCHAR) * PATHCCH_MAX_CCH;
	HKEY hKey = NULL;
	WCHAR *data = nullptr;
	LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);

	if (ret == ERROR_SUCCESS) //Check registry for 64-bit Windows installation
	{
		data = newDynamicArray();
		ret = RegQueryValueExW(hKey, L"InstallPath", NULL, NULL, reinterpret_cast<LPBYTE>(data), &cbData);
		RegCloseKey(hKey);
	}

	if (ret == ERROR_FILE_NOT_FOUND) //Then check for 32-bit if unsuccessful
	{
		ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			if (data == nullptr)
				data = newDynamicArray();

			ret = RegQueryValueExW(hKey, L"InstallPath", NULL, NULL, reinterpret_cast<LPBYTE>(data), &cbData);
			RegCloseKey(hKey);
		}
	}

	if (ret == ERROR_SUCCESS)
	{
		if (data[cbData - 1] != '\0')
			data[cbData] = '\0';

		steamDir = data;
		testDir = steamDir + L"\\steamapps";
	}

	if (data != nullptr)
		deleteDynamicArray(data);

	return ret == ERROR_SUCCESS;
}

bool FindCsgo::bCheckCsgoInstall()
{
	bool bFoundCsgo = false;
	wifstream manifest;

	manifest.open(testDir + L"\\appmanifest_730.acf");

	if (!manifest.fail())
	{
		WCHAR searchResult[1][MAX_PATH];

		//Verify CS:GO installation directory listed in manifest file contents
		if (textFileOps.parseTextFile(wstring(L"\"installdir\""), manifest, searchResult, 1, L"\t\"\0", 2) != 0)
		{
			DWORD nBufferLength;

			testDir += wstring(L"\\common\\") + searchResult[0];
			nBufferLength = GetFullPathNameW((testDir + L"\\csgo.exe").c_str(), 0, nullptr, NULL);

			if (nBufferLength != 0)
			{
				WCHAR *lpBuffer = new WCHAR[nBufferLength + 4];

				wmemcpy_s(lpBuffer, nBufferLength + 4, L"\\\\?\\", 4); //Prefix to permit extended-length path

				if (GetFullPathNameW((testDir + L"\\csgo.exe").c_str(), nBufferLength, lpBuffer + 4, NULL) == nBufferLength - 1)
				{
					HANDLE csgo = CreateFileW((testDir + L"\\csgo.exe").c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
											  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //Check for existence of CS:GO's exe

					if (csgo != INVALID_HANDLE_VALUE && csgo != NULL || GetLastError() == ERROR_SHARING_VIOLATION)
					{
						bFoundCsgo = true;
						CloseHandle(csgo);
					}
				}

				if (nBufferLength != 0)
					delete[] lpBuffer;

				lpBuffer = nullptr;
			}
		}

		manifest.close();
	}

	return bFoundCsgo;
}

bool FindCsgo::bSearchSteamLibs()
{
	bool bFoundCsgo = false;
	wifstream libFile;

	libFile.open(testDir + L"\\libraryfolders.vdf"); //File contains other user defined Steam libraries if they exist

	if (!libFile.fail())
	{
		int i = 1;

		while (i < 10 && !bFoundCsgo)
		{
			const wstring searchTerm = { L'\"', static_cast<WCHAR>(i + static_cast<int>(L'0')), L'\"', L'\0' };
			WCHAR searchResult[1][MAX_PATH];

			if (textFileOps.parseTextFile(searchTerm, libFile, searchResult, 1, L"\t\"\0", 2) == 0)
				break; //No other user-defined Steam libraries found to check for CS:GO
			else
			{
				testDir += wstring(searchResult[0]) + L"\\steamapps"; //Found user-defined Steam library to check for CS:GO
				++i;

				if (bCheckCsgoInstall())
					bFoundCsgo = true;
			}
		}

		libFile.close();
	}

	return bFoundCsgo;
}

wstring FindCsgo::getTestDir() const
{
	return testDir;
}

wchar_t *const FindCsgo::newDynamicArray() const
{
	wchar_t *data = new wchar_t[PATHCCH_MAX_CCH];

	return data;
}

void FindCsgo::deleteDynamicArray(wchar_t *&data) const
{
	delete[] data;
	data = nullptr;
}

} //namespace csgo
} //namespace sircat
