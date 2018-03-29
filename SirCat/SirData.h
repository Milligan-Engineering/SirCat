#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class SirData : public Archive
{
private: //private first because public member function need the private constants
	static const int k_num_weap = 26; //Number of relevant weapons
	static const int k_num_attr = 21; //Number of relevant attributes

	bool bIsDigit(const char character);
	//Precondition: 
	//Postcondition: 

	bool bStaticVarsInitialized;
	static string weapNames[k_num_weap];
	static string weapAlts[k_num_weap]; //Names for usable alt firing modes
	static string attrNames[k_num_attr];
	string sirData[k_num_weap][k_num_attr];
public:
	SirData(const char setCsvName[]);
	//Precondition: 
	//Postcondition: 

	SirData();
	//Precondition: 
	//Postcondition: 

	~SirData() = default;

	bool getBStaticVarsInitialized();
	//Precondition: 
	//Postcondition: 

	bool bReadWeapFile(const string csgoDir);
	//Precondition: 
	//Postcondition: 

	bool bCheckArchive(SirData &newSir);
	//Precondition: 
	//Postcondition:

	void readArchive();
	//Precondition: 
	//Postcondition:

	bool bWriteArchiveFile(SirData &newSir);
	//Precondition: 
	//Postcondition:
};
