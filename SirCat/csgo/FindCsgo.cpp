#ifndef __STDC_WANT_LIB_EXT1__
	#define __STDC_WANT_LIB_EXT1__ 1 //wmemcpy_s
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
//Using wmemcpy_s
using std::wifstream;
using std::wstring;
/*Using CloseHandle, CreateFileW, DWORD, ERROR_FILE_NOT_FOUND, ERROR_SHARING_VIOLATION, ERROR_SUCCESS, FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ, GENERIC_READ, GetFullPathNameW, GetLastError, HANDLE, HKEY, HKEY_LOCAL_MACHINE, INVALID_HANDLE_VALUE,
		KEY_QUERY_VALUE, LONG, LPBYTE, OPEN_EXISTING, RegCloseKey, RegOpenKeyExW, RegQueryValueExW, WCHAR*/
//Using PATHCCH_MAX_CCH

bool FindCsgo::bFetchSteamDir(wstring &steamDir)
{
	HKEY hKey = NULL;
	LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0ul, KEY_QUERY_VALUE, &hKey);

	DWORD cbData = sizeof(WCHAR) * PATHCCH_MAX_CCH;
	WCHAR *data = nullptr;

	if (ret == ERROR_SUCCESS) //Check registry for 64-bit Windows installation
	{
		data = newDynamicArray();
		ret = RegQueryValueExW(hKey, L"InstallPath", NULL, NULL, reinterpret_cast<LPBYTE>(data), &cbData);
		RegCloseKey(hKey);
	}

	if (ret == ERROR_FILE_NOT_FOUND) //Then check for 32-bit if unsuccessful
	{
		ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0ul, KEY_QUERY_VALUE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			if (data == nullptr)
				data = newDynamicArray();

			ret = RegQueryValueExW(hKey, L"InstallPath", NULL, NULL, reinterpret_cast<LPBYTE>(data), &cbData);
			RegCloseKey(hKey);
		}
	}

	if (ret == ERROR_SUCCESS && data != nullptr)
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
		wstring searchResult[1];

		//Verifies CS:GO installation directory listed in manifest file contents
		if (textFileOps.parseTextFile(L"\"installdir\"", manifest, searchResult, 1, L"\t\"\0", 2) != 0)
		{
			DWORD nBufferLength;

			testDir += L"\\common\\" + searchResult[0];
			nBufferLength = GetFullPathNameW((testDir + L"\\csgo.exe").c_str(), 0ul, nullptr, NULL);

			if (nBufferLength != 0ul)
			{
				WCHAR *lpBuffer = new WCHAR[nBufferLength + 4ul];

				wmemcpy_s(lpBuffer, nBufferLength + 4ul, L"\\\\?\\", 4u); //Prefix permits extended-length path

				if (GetFullPathNameW((testDir + L"\\csgo.exe").c_str(), nBufferLength, lpBuffer + 4, NULL)
					== nBufferLength - 1ul)
				{
					HANDLE csgo = CreateFileW((testDir + L"\\csgo.exe").c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
											  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //Checks for existence of CS:GO's exe

					if (csgo != INVALID_HANDLE_VALUE && csgo != NULL || GetLastError() == ERROR_SHARING_VIOLATION)
					{
						bFoundCsgo = true;
						CloseHandle(csgo);
					}
				}

				if (nBufferLength != 0ul)
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
			wstring searchResult[1];

			if (textFileOps.parseTextFile(searchTerm, libFile, searchResult, 1, L"\t\"\0", 2) == 0)
				break; //No other user-defined Steam libraries to check for CS:GO
			else
			{
				testDir += searchResult[0] + L"\\steamapps"; //User-defined Steam library to check for CS:GO
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
	wchar_t *array = new wchar_t[PATHCCH_MAX_CCH];

	return array;
}

void FindCsgo::deleteDynamicArray(wchar_t *&array) const
{
	delete[] array;
	array = nullptr;
}

} //namespace csgo
} //namespace sircat
