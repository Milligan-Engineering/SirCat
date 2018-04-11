#pragma once

#include "..\TextFileOps\TextFileOps.h"
#include <iosfwd>
#include <string>

using namespace std;

class Archive
{
public:
	bool bMakeObjArchive(const int numHeaders, wstring *headers[], const int sliceSize[], const bool sliceIsRow[],
		const int numSlice[]);
	//Precondition: 
	//Postcondition: 

	wstring getCsvName();
	//Precondition: 
	//Postcondition: 
protected:
	Archive() = default;
	~Archive() = default;

	bool bCheckArchiveRow(const wstring row1[], const wstring row2[], int &j);
	//Precondition: 
	//Postcondition: 

	void readArchiveRow(wstring archiveRow[], const int rowNum);
	//Precondition: rowNum is the row number in the archive file to be parsed.
	//Postcondition: Parses a row of data from the archive file and stores it in array archiveRow.

	void writeArchiveFileRow(const wstring newRow[]);
	//Precondition: 
	//Postcondition: 

	wifstream &getInArchive();
	//Precondition: 
	//Postcondition: 

	wofstream &getOutArchive();
	//Precondition: 
	//Postcondition: 

	TextFileOps textFileOps;
	wstring csvName;
	int numColumns;
};
