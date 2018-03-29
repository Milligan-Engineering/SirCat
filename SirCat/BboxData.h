#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class BboxData : public Archive
{
private: //private first because public member function need the private constants
	static const int k_num_model = 21; //Number of relevant player hitbox models
	static const int k_num_attr = 7; //Number of relevant hitbox attributes

	bool bStaticVarsInitialized;
	static string modelNames[k_num_model];
	static string attrNames[k_num_attr];
	string bboxData[k_num_model][k_num_attr];
public:
	BboxData(const char setCsvName[]);
	//Precondition: 
	//Postcondition: 

	BboxData();
	//Precondition: 
	//Postcondition: 

	~BboxData() = default;

	bool getBStaticVarsInitialized();
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

	bool bCheckArchive(BboxData &newSir);
	//Precondition: 
	//Postcondition:

	void readArchive();
	//Precondition: 
	//Postcondition:

	bool bWriteArchiveFile(BboxData &newBbox);
	//Precondition: 
	//Postcondition:
};
