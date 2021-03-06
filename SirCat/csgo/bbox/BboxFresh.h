#pragma once

#include "..\..\targetver.h"

#include "..\GameData.h"	//Using sircat::csgo::GameData
#include "BboxArchive.h"	//Using sircat::csgo::bbox::BboxArchive
#include <string>			//Using std::wstring
#include <Windows.h>		//Using BOOL, CALLBACK, DWORD, HANDLE, HWND, LPARAM, PROCESS_INFORMATION, WCHAR, WIN32_FIND_DATAW

namespace sircat {
namespace csgo {
namespace bbox {

class BboxFresh : public GameData
{
public:
	struct WinInfo;
	struct ChildInfo;

	BboxFresh(const BboxArchive &bboxArchive) : GameData(bboxArchive, nullptr) {};
	BboxFresh &operator= (const BboxFresh &other) = delete;

	bool bUnpackModels(const std::wstring csgoDir) const;
	//Precondition: csgoDir is a directory containing a valid install of CS:GO; HLExtract files are in ./HLExtract directory
	//Postcondition: Invokes HLExtract to unpack player model files into ./legacy directory and waits for it to exit
		//Returns true if HLExtract was successfully started, and false otherwise

	bool bDecompileModels() const;
	//Precondition: Player model files have been unpacked into ./legacy directory; Crowbar files are in ./Crowbar directory
	//Postcondition: Starts an instance of Crowbar to decompile the models, automates its operation, and waits for it to exit
		//Returns true if Crowbar was successfully started, and false otherwise

	bool bWaitForCrowbarWindow(const PROCESS_INFORMATION &pi) const;
	//Precondition: pi is populated with information from a running instance of Crowbar; see Precondition for bDecompileModels
	//Postcondition: An instance of Crowbar is automated to decompile the models and waited on to finish
		//Returns true if Crowbar's working directory was successfully found, and false otherwise

	bool bAutomateCrowbar(const HWND &winInfoHwnd, const HWND &guiHwndFocus) const;
	//Precondition: winInfoHwnd/guiHwndFocus contain HWNDs of active Crowbar window; see Precondition for bDecompileModels
	//Postcondition: An instance of Crowbar is automated to decompile the models and waited on to finish
		//Returns true if Crowbar's working directory was successfully found, and false otherwise

	bool bReadModelFiles(const bool bCleanLegacyDir = false);
	//Precondition: Player model files have been decompiled into ./legacy directory;
	//Postcondition: Parses the model files and reads bbox data into a BboxData object's data array
		//Returns true if data array was fully filled with bbox data, and false otherwise
private:
	bool bCreateProcess(const WCHAR *const applicationName, WCHAR *const commandLine,
						PROCESS_INFORMATION *const pPi = nullptr, bool bWaitForExit = true) const;

	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

	void fetchModelFileDir(const DWORD nBufferLength, HANDLE &hFind, WCHAR *&lpBuffer, WCHAR *&lpFileName,
						   WIN32_FIND_DATAW &FindFileData) const;
	//Postcondition: Attempts to find the extended-length path of the ./legacy directory, and the normal path if unsuccessful
		//hFind receives HANDLE from FindFirstFileW; *lpBuffer receives the extended-length path if one was found
		//lpFileName receives the extended-length path if one was found, and the normal path otherwise
		//FindFileData is populated by FindFirstFileW.

	int fetchModelBboxData(int &i, WIN32_FIND_DATAW &FindFileData);
	//Precondition: i is modifiable and contains a valid data array index
		//FindFileData is populated with information pertaining to the decompiled model file to be parsed
	//Postcondition: Fills a row of data array with the model's bbox data if the model file is opened successfully
		//Returns -1 if the file failed to open, and 1 otherwise
};

} //namespace bbox
} //namespace csgo
} //namespace sircat
