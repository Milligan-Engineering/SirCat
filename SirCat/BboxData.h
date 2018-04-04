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

class BboxData : public Archive
{
private: //private first because public member function need the private constants
	static const int k_num_model = 21; //Number of relevant player hitbox models
	static const int k_num_attr = 7; //Number of relevant hitbox attributes

	static wstring modelNames[k_num_model];
	static wstring attrNames[k_num_attr];
	wstring bboxData[k_num_model][k_num_attr];
public:
	BboxData();
	//Precondition: 
	//Postcondition: 

	BboxData(const WCHAR setCsvName[]);
	//Precondition: 
	//Postcondition: 

	~BboxData() = default;

	bool bReadModelFiles();
	//Precondition: 
	//Postcondition: 

	bool bUnpackModels();
	//Precondition: 
	//Postcondition: 

	bool bDecompileModels();
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
};
