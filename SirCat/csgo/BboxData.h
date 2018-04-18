#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class BboxData : public Archive
{
public:
	BboxData();
	//Precondition: 
	//Postcondition: 

	~BboxData() = default;

	bool bMakeBboxObjArchive(const wstring csvName);
	//Precondition: 
	//Postcondition: 

	bool bUnpackModels(const wstring csgoDir);
	//Precondition: 
	//Postcondition: 

	bool bDecompileModels();
	//Precondition: 
	//Postcondition: 

	bool bReadModelFiles();
	//Precondition: 
	//Postcondition: 

	bool bCheckArchive(BboxData &newBbox, wstring &badRowName, wstring &badColName, wstring &badNewVal, wstring &badArchiveVal);
	//Precondition: 
	//Postcondition: 

	void readArchive();
	//Precondition: 
	//Postcondition: 

	bool bWriteArchiveFile(BboxData &newBbox);
	//Precondition: 
	//Postcondition: 
private:
	bool bCreateProcess(wstring applicationName, wstring commandLineWString,
		unsigned long *pDwProcessId = nullptr, unsigned long dwCreationFlags = 0);
	//Precondition: 
	//Postcondition: 
};
