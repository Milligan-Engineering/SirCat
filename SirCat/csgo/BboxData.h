#pragma once

#ifndef STRICT
	#define STRICT 1
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
#endif //WIN32_LEAN_AND_MEAN

#include "Archive.h"
#include <string>
#include <Windows.h>

using namespace std;

class BboxData : public Archive
{
public:
	BboxData() = default;

	BboxData(const wstring csvName) : Archive(csvName) {};
	//Precondition: 
	//Postcondition: 

	BboxData(const BboxData &otherBboxData) : Archive(otherBboxData, nullptr) {};
	//Precondition: 
	//Postcondition: 

	~BboxData() = default;

	bool bUnpackModels(const wstring csgoDir) const;
	//Precondition: 
	//Postcondition: 

	bool bDecompileModels() const;
	//Precondition: 
	//Postcondition: 

	bool bWaitForCrowbarWindow(const PROCESS_INFORMATION &pi) const;
	//Precondition: 
	//Postcondition: 

	bool bAutomateCrowbar(const HWND &winInfoHwnd, const HWND &guiHwndFocus) const;
	//Precondition: 
	//Postcondition: 

	bool bReadModelFiles(const bool bCleanLegacyDir = false);
	//Precondition: 
	//Postcondition: 
private:
	struct WinInfo;
	struct ChildInfo;

	bool bCreateProcess(const WCHAR *const applicationName, WCHAR *const commandLine,
		PROCESS_INFORMATION *pPi = nullptr, bool bWaitForExit = true) const;
	//Precondition: 
	//Postcondition: 

	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	//Precondition: 
	//Postcondition: 

	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);
	//Precondition: 
	//Postcondition: 

	void fetchModelFileDir(const int nBufferLength, HANDLE &hFind, WCHAR *&lpBuffer, WCHAR *&lpFileName,
		WIN32_FIND_DATAW &FindFileData) const;
	//Precondition: 
	//Postcondition: 

	int fetchModelBboxData(int &i, WIN32_FIND_DATAW &FindFileData);
	//Precondition: 
	//Postcondition: 
};
