#pragma once

#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "TextFileOps.h"
#include <string>
#include <Windows.h>

class FindCsgo
{
public:
	static FindCsgo &inst();
	//Precondition: 
	//Postcondition: 

	bool bFetchSteamDir(wstring &steamDir);
	//Postcondition: If valid registry value for Steam installation is found, testDir is filled with the installation directory.
		//Returns true if valid registry value is found, and false otherwise.

	bool bCheckCsgoInstall();
	//Postcondition: If a CS:GO manifest file is found in testDir, it is parsed for the CS:GO installation directory.
		//If the installation directory is parsed from the manifest file, testDir is filled with it.
		//Returns true if the installation directory is parsed from the manifest file, and false otherwise.

	bool bSearchSteamLibs();
	//Postcondition: If Steam library file is found in testDir, it is parsed to enumerate up to 9 alt library paths to search.
		//testDir is filled with one alt library path at a time and passed to the bCheckCsgoInstall function.
		//Returns true and stops enumerating if bCheckCsgoInstall returns true for an alt library path, and false otherwise.

	wstring getTestDir();
	//Precondition: 
	//Postcondition: 
private:
	FindCsgo() = default;
	~FindCsgo() = default;

	WCHAR intDigitToWchar(const int intDigit);
	//Precondition: 
	//Postcondition: 

	WCHAR testDir[TextFileOps::k_max_path];
};
