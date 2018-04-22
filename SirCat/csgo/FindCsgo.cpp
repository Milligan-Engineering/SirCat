#ifndef STRICT
	#define STRICT 1
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
#endif //WIN32_LEAN_AND_MEAN

#include "FindCsgo.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>
#include <wchar.h>
#include <Windows.h>

using namespace std;

FindCsgo::FindCsgo()
{
	textFileOps = new TextFileOps;
}

FindCsgo::~FindCsgo()
{
	delete textFileOps;
	textFileOps = nullptr;
}

bool FindCsgo::bFetchSteamDir(wstring &steamDir)
{
	HKEY hKey = NULL;
	//DWORD type = REG_SZ;
	WCHAR *data = nullptr;
	DWORD cbData = sizeof(WCHAR) * 32767;
	LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);

	if (ret == ERROR_SUCCESS) //Check registry for 64-bit Windows installation
	{
		data = static_cast<WCHAR *>(newDynamicArray());
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
		testDir = steamDir + static_cast<wstring>(L"\\steamapps");
	}

	if (data != nullptr)
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
		if (textFileOps->parseTextFile(static_cast<wstring>(L"\"installdir\""), manifest, searchResult, 1, L"\t\"\0", 2) != 0)
		{
			DWORD nBufferLength;

			testDir += static_cast<wstring>(L"\\common\\") + static_cast<wstring>(searchResult[0]);
			nBufferLength = GetFullPathNameW((testDir + static_cast<wstring>(L"\\csgo.exe")).c_str(), 0, nullptr, NULL);

			if (nBufferLength != 0)
			{
				WCHAR *lpBuffer = new WCHAR[nBufferLength + 4];

				wmemcpy_s(lpBuffer, nBufferLength + 4, L"\\\\?\\", 4); //Prefix to permit extended-length path

				if (GetFullPathNameW((testDir + static_cast<wstring>(L"\\csgo.exe")).c_str(), nBufferLength, lpBuffer + 4, NULL)
					== nBufferLength - 1)
				{
					HANDLE csgo = CreateFileW((testDir + static_cast<wstring>(L"\\csgo.exe")).c_str(), GENERIC_READ,
						FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //Check for existence of CS:GO's exe

					if (csgo != INVALID_HANDLE_VALUE && csgo != NULL || GetLastError() == ERROR_SHARING_VIOLATION)
					{
						bFoundCsgoInstall = true;
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

			if (textFileOps->parseTextFile(static_cast<wstring>(searchTerm), libFile, searchResult, 1, L"\t\"\0", 2) != 0)
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

wstring FindCsgo::getTestDir() const
{
	return testDir;
}

wchar_t *FindCsgo::newDynamicArray() const
{
	wchar_t *data = new WCHAR[32767];

	return data;
}

void FindCsgo::deleteDynamicArray(wchar_t *&data) const
{
	delete[] data;
	data = nullptr;
}
