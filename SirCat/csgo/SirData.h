#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class SirData : public Archive
{
private:
	static const int k_num_weap = 26; //Number of relevant weapons
	static const int k_num_attr = 21; //Number of relevant attributes

	static bool bArchiveObjMade;
	static wstring weapNames[k_num_weap];
	static wstring weapAlts[k_num_weap]; //Names for usable alt firing modes
	static wstring attrNames[k_num_attr];
	wstring sirData[k_num_weap][k_num_attr];
public:
	SirData();
	//Precondition: 
	//Postcondition: 

	~SirData() = default;

	bool bMakeSirObjArchive(const wstring csvName);
	//Precondition: 
	//Postcondition: 

	bool bReadWeapFile(const wstring csgoDir);
	//Precondition: 
	//Postcondition: 

	bool bCheckArchive(SirData &newSir, wstring &badRowName, wstring &badColName, wstring &badNewVal,
		wstring &badArchiveVal);
	//Precondition: 
	//Postcondition: 

	void readArchive();
	//Precondition: 
	//Postcondition: 

	bool bWriteArchiveFile(SirData &newSir);
	//Precondition: 
	//Postcondition: 
};
