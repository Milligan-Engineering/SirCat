#include "Archive.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>

using namespace std;

void Archive::temp(const Archive *const otherArchive, bool b1, bool b2, bool b3, bool b4, bool b5)///////////////////////////
{///////////////////////////////////////////////
	if (b1)///////////////////////////////////////////////
		numRows = otherArchive->numRows;///////////////////////////////////////////////
	if (b2)///////////////////////////////////////////////
		numColumns = otherArchive->numColumns; ///////////////////////////////////////////////
	if (b3) //////////////////////////////////////
	{///////////////////////////////////////////////
		rowHeaders = new wstring[numRows];///////////////////////////////////////////////
		for (int i = 0; i < numRows; ++i)///////////////////////////////////////////////
			rowHeaders[i] = otherArchive->rowHeaders[i];///////////////////////////////////////////////
	}///////////////////////////////////////////////
	if (b4) ///////////////////////////////////////////////
	{///////////////////////////////////////////////
		columnHeaders = new wstring[numColumns];////////////////////////////////////////////
		for (int j = 0; j < numColumns; ++j)////////////////////////////////////////////
			columnHeaders[j] = otherArchive->columnHeaders[j]; ///////////////////////////////////////////////
	}///////////////////////////////////////////////
	if (b5)////////////////////////////////////////////
	{////////////////////////////////////////////
		data = new wstring*[numRows];////////////////////////////////////////////
		////////////////////////////////////////////
		for (int i = 0; i < numRows; ++i)////////////////////////////////////////////
			data[i] = new wstring[numColumns];////////////////////////////////////////////
	}////////////////////////////////////////////
}////////////////////////////////////////////

int Archive::compareArchives(const Archive *const otherArchive, const bool bGetNonMatchSize, NonMatch **nonMatches) const
{
	int ret = 0;
	int retHold = 0;

	if (!bGetNonMatchSize)
	{
		if ((ret = compareArchives(otherArchive, true)) > 0) //Single = is intentional
		{
			*nonMatches = new NonMatch[ret];
			retHold = ret;
		}
		//******handle deleting these arrays in the class somewhere******
	}

	for (int otherI = 0; otherI < otherArchive->numRows; ++otherI)
	{
		for (int i = 1; i < numRows; ++i)
		{
			if (otherArchive->rowHeaders[otherI] == rowHeaders[i])
			{
				for (int j = 0; j < numColumns; ++j)
				{
					if (otherArchive->data[otherI][j] != data[i][j])
					{
						if (bGetNonMatchSize)
							++ret;
						else if (nonMatches != nullptr)
						{
							--ret;
							(*nonMatches)[ret].otherRowHeader = otherArchive->rowHeaders[otherI];
							(*nonMatches)[ret].commonColumnHeader = columnHeaders[j];
							(*nonMatches)[ret].otherDatum = otherArchive->data[otherI][j];
							(*nonMatches)[ret].datum = data[i][j];
						}
					}
				}

				break; //otherArchive.rowHeaders[otherI] has been matched and compared
			}
			else if (i == numRows - 1)
			{
				for (int j = 0; j < numColumns; ++j)
				{
					if (bGetNonMatchSize)
						++ret;
					else if (nonMatches != nullptr)
					{
						--ret;
						(*nonMatches)[ret].otherRowHeader = otherArchive->rowHeaders[otherI];
						(*nonMatches)[ret].commonColumnHeader = columnHeaders[j];
						(*nonMatches)[ret].otherDatum = otherArchive->data[otherI][j];
						(*nonMatches)[ret].datum = wstring();
					}
				}
			}
		}
	}

	if (nonMatches != nullptr && retHold > 0)
		ret = retHold;

	return ret;
}

bool Archive::bWriteArchiveFile(const wstring csvName)
{
	bool bSuccess = false;

	if (!csvName.empty())
		Archive::csvName = csvName;

	getOutArchive().open(csvName);

	if (!getOutArchive().fail())
	{
		writeArchiveFileRow(columnHeaders);
		getOutArchive() << endl;

		for (int i = 0; i < numRows; ++i)
		{
			getOutArchive() << rowHeaders[i];
			writeArchiveFileRow(data[i]);
			getOutArchive() << endl;
		}

		bSuccess = true;
		getOutArchive().close();
	}

	return bSuccess;
}

wstring Archive::getCsvName() const
{
	return csvName;
}

bool Archive::getBSuccessUseCsv() const
{
	return bSuccessUseCsv;
}

Archive::Archive()
{
	//Default values for when the default constructor is not called by the other delegating constructor
	numRows = 0;
	numColumns = 0;
	rowHeaders = nullptr;
	columnHeaders = nullptr;
	data = nullptr;
	csvName = wstring();
	textFileOps = new TextFileOps;
	inArchive = new wifstream;
	outArchive = new wofstream;
	bSuccessUseCsv = false;
}

Archive::Archive(const wstring csvName) : Archive()
{
	numRows = textFileOps->fetchNumRows(getInArchive(), csvName) - 1;
	numColumns = textFileOps->fetchNumColumns(getInArchive(), csvName) - 1;
	rowHeaders = new wstring[numRows];
	columnHeaders = new wstring[numColumns];
	data = new wstring*[numRows];

	for (int i = 0; i < numRows; ++i)
		data[i] = new wstring[numColumns];

	Archive::csvName = csvName;
	bSuccessUseCsv = true; //Default to true because useCsv sets bSuccessUseCsv to false if there is an error
	useCsv();
}

Archive::~Archive()
{
	if (rowHeaders != nullptr)
	{
		delete[] rowHeaders;
		rowHeaders = nullptr;
	}

	if (columnHeaders != nullptr)
	{
		delete[] columnHeaders;
		columnHeaders = nullptr;
	}

	if (data != nullptr)
	{
		for (int i = 0; i < numRows; ++i)
			delete[] data[i];

		delete[] data;
		data = nullptr;
	}

	delete textFileOps;
	textFileOps = nullptr;
	delete inArchive;
	inArchive = nullptr;
	delete outArchive;
	outArchive = nullptr;
}

wifstream &Archive::getInArchive() const
{
	return *inArchive;
}

wofstream &Archive::getOutArchive() const
{
	return *outArchive;
}

void Archive::useCsv()
{
	wstring *headers[2] = { rowHeaders, columnHeaders };
	const int sliceSize[2] = { numRows, numColumns };
	const bool sliceIsRow[2] = { false, true };
	const int numSlice[2] = { 1, 1 };

	for (int i = 0; i < 2; ++i)
	{
		if (textFileOps->fetchDelimitedSlice(getInArchive(), csvName, headers[i], sliceSize[i], sliceIsRow[i], 2, L',',
			numSlice[i]) != sliceSize[i])
		{
			bSuccessUseCsv = false;
			break;
		}
	}

	for (int i = 0; i < numRows; ++i)
		textFileOps->fetchDelimitedSlice(getInArchive(), csvName, data[i], numColumns, true, 2, L',', i + 2);
}

void Archive::writeArchiveFileRow(const wstring newRow[])
{
	for (int j = 0; j < numColumns; ++j)
		getOutArchive() << L',' << newRow[j];
}
