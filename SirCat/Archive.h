#pragma once

#include <iosfwd> //Forward declares ifstream without #include <fstream>
#include <string>

using namespace std;

class Archive
{
protected:
	Archive() = default;
	~Archive() = default;

	ifstream &getInArchive();
	//Precondition: 
	//Postcondition:

	ofstream &getOutArchive();
	//Precondition: 
	//Postcondition:

	bool bCheckArchiveRow(const string rowName, const string columnNames[], const string newRow[], const string archiveRow[]);
	//Precondition: 
	//Postcondition: 

	void readArchiveRow(string archiveRow[], const int rowNum);
	//Precondition: rowNum is the row number in the archive file to be parsed.
	//Postcondition: Parses a row of data from the archive file and stores it in array archiveRow.

	void writeArchiveFileRow(const string newRow[]);
	//Precondition: 
	//Postcondition: 

	int numColumns;
	string csvName;
};
