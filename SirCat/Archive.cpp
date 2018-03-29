#include "Archive.h"
#include "TextFileOps.h"
#include <fstream>
#include <iostream>
#include <string>

ifstream &Archive::getInArchive()
{
	static ifstream inArchive;
	return inArchive;
}

ofstream &Archive::getOutArchive()
{
	static ofstream outArchive;
	return outArchive;
}

bool Archive::bCheckArchiveRow(const string rowName, const string columnNames[], const string newRow[], const string archiveRow[])
{
	bool bUpdate = false;

	for (int j = 0; j < numColumns && !bUpdate; ++j) //&& !bUpdate will terminate the loop after first mismatch
	{
		if (newRow[j] != archiveRow[j])
		{
			bUpdate = true;

			cout << "Data mismatch detected for " << rowName << " " << columnNames[j] << ":\n";
			cout << "Value from CS:GO's game files: " << newRow[j] << endl;
			cout << "Value from archive file: " << archiveRow[j] << endl << endl;
		}
	}

	return bUpdate;
}

void Archive::readArchiveRow(string archiveRow[], const int rowNum)
{
	TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, archiveRow, numColumns, true, 2, ',', rowNum);
}

void Archive::writeArchiveFileRow(const string newRow[])
{
	for (int j = 0; j < numColumns; ++j)
		getOutArchive() << ',' << newRow[j];
}
