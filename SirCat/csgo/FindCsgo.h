#pragma once

#include "..\util\TextFileOps.h"
#include <string>

namespace sircat {
namespace csgo {

class FindCsgo
{
public:
	FindCsgo() = default;
	~FindCsgo() = default;

	bool bFetchSteamDir(std::wstring &steamDir);

	bool bCheckCsgoInstall();
	//Postcondition: If a CS:GO manifest file is found in testDir, it is parsed for the CS:GO installation directory
		//If the installation directory is parsed from the manifest file, testDir is filled with it
		//Returns true if the installation directory is parsed from the manifest file, and false otherwise

	bool bSearchSteamLibs();
	//Postcondition: If Steam library file is found in testDir, it is parsed to enumerate up to 9 alt library paths to search
		//testDir is filled with one alt library path at a time and passed to the bCheckCsgoInstall function
		//Returns true and stops enumerating if bCheckCsgoInstall returns true for an alt library path, and false otherwise

	std::wstring getTestDir() const;
private:
	wchar_t *const newDynamicArray() const;

	void deleteDynamicArray(wchar_t *&data) const;

	sircat::util::TextFileOps textFileOps;
	std::wstring testDir;
};

} //namespace csgo
} //namespace sircat
