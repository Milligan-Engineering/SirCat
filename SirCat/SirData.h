#pragma once

#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "Archive.h"
#include <string>
#include <Windows.h>

using namespace std;

class SirData : public Archive
{
private: //private first because public member function need the private constants
	static const int k_num_weap = 26; //Number of relevant weapons
	static const int k_num_attr = 21; //Number of relevant attributes

	bool bIsDigit(const WCHAR character);
	//Precondition: 
	//Postcondition: 

	static wstring weapNames[k_num_weap];
	static wstring weapAlts[k_num_weap]; //Names for usable alt firing modes
	static wstring attrNames[k_num_attr];
	wstring sirData[k_num_weap][k_num_attr];
public:
	SirData();
	//Precondition: 
	//Postcondition: 

	SirData(const WCHAR setCsvName[]);
	//Precondition: 
	//Postcondition: 

	~SirData() = default;

	bool bReadWeapFile(const wstring csgoDir);
	//Precondition: 
	//Postcondition: 

	bool bCheckArchive(SirData &newSir, wstring &badRowName, wstring &badColName, wstring &badNewVal, wstring &badArchiveVal);
	//Precondition: 
	//Postcondition: 

	void readArchive();
	//Precondition: 
	//Postcondition: 

	bool bWriteArchiveFile(SirData &newSir);
	//Precondition: 
	//Postcondition: 
};
