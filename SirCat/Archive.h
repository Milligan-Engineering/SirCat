#pragma once

#include <iosfwd> //Forward declares ifstream without #include <fstream>
#include <string>

using namespace std;

class Archive
{
public:
	wstring getCsvName();
	//Precondition: 
	//Postcondition: 

	bool getBStaticVarsInitialized();
	//Precondition: 
	//Postcondition: 
protected:
	Archive() = default;
	~Archive() = default;

	wifstream &getInArchive();
	//Precondition: 
	//Postcondition: 

	wofstream &getOutArchive();
	//Precondition: 
	//Postcondition: 

	bool bCheckArchiveRow(const wstring rowName, const wstring columnNames[], const wstring newRow[],
		const wstring archiveRow[], int &j);
	//Precondition: 
	//Postcondition: 

	void readArchiveRow(wstring archiveRow[], const int rowNum);
	//Precondition: rowNum is the row number in the archive file to be parsed.
	//Postcondition: Parses a row of data from the archive file and stores it in array archiveRow.

	void writeArchiveFileRow(const wstring newRow[]);
	//Precondition: 
	//Postcondition: 

	wstring csvName;
	bool bStaticVarsInitialized;
	int numColumns;
};
