#include "Archive.h"
#include "TextFileOps.h"
#include <fstream>
#include <string>

using namespace std;

bool Archive::bMakeObjArchive(const int numHeaders, wstring *headers[], const int sliceSize[], const bool sliceIsRow[],
	const int numSlice[])
{
	bool bSuccess = true;

	for (int i = 0; i < numHeaders; ++i)
	{
		if (TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, headers[i], sliceSize[i], sliceIsRow[i], 2, L',',
			numSlice[i]) != sliceSize[i])
		{
			bSuccess = false;
			break;
		}
	}

	return bSuccess;
}

wstring Archive::getCsvName()
{
	return csvName;
}

wifstream &Archive::getInArchive()
{
	static wifstream inArchive;
	return inArchive;
}

wofstream &Archive::getOutArchive()
{
	static wofstream outArchive;
	return outArchive;
}

bool Archive::bCheckArchiveRow(const wstring rowName, const wstring colNames[], const wstring newRow[],
	const wstring archiveRow[],	int &j)
{
	bool bUpdate = false;

	for (j = 0; j < numColumns; ++j)
	{
		if (newRow[j] != archiveRow[j])
		{
			bUpdate = true;
			break; //Terminate the loop after first mismatch
		}
	}

	return bUpdate;
}

void Archive::readArchiveRow(wstring archiveRow[], const int rowNum)
{
	TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, archiveRow, numColumns, true, 2, L',', rowNum);
}

void Archive::writeArchiveFileRow(const wstring newRow[])
{
	for (int j = 0; j < numColumns; ++j)
		getOutArchive() << L',' << newRow[j];
}
