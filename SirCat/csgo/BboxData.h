#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class BboxData : public Archive
{
private:
	static const int k_num_model = 21; //Number of relevant player hitbox models
	static const int k_num_attr = 7; //Number of relevant hitbox attributes

	static bool bArchiveObjMade;
	static wstring modelNames[k_num_model];
	static wstring attrNames[k_num_attr];
	wstring bboxData[k_num_model][k_num_attr];
public:
	BboxData();
	//Precondition: 
	//Postcondition:

	~BboxData() = default;

	bool bMakeBboxObjArchive(const wstring csvName);
	//Precondition: 
	//Postcondition: 

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
