#include "Archive.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>

using namespace std;

int Archive::compareArchives(const Archive &otherArchive, const bool bGetNonMatchSize)
{
	int ret = 0;

	if (!bGetNonMatchSize)
		allocNonMatches(otherArchive, ret);

	for (int otherI = 0; otherI < otherArchive.numRows; ++otherI) //Index for first dimension of otherArchive data array
	{
		for (int i = 0; i < numRows; ++i) //Index for first dimension of this* Archive data array
		{
			if (otherArchive.rowHeaders[otherI] == rowHeaders[i])
			{
				for (int j = 0; j < numColumns; ++j) //Index for second dimension of both data arrays
				{
					if (otherArchive.data[otherI][j] != data[i][j])
					{
						if (bGetNonMatchSize)
							++ret;
						else if (nonMatches != nullptr)
						{
							--ret;
							nonMatches[ret].otherRowHeader = otherArchive.rowHeaders[otherI];
							nonMatches[ret].commonColumnHeader = columnHeaders[j];
							nonMatches[ret].otherDatum = otherArchive.data[otherI][j];
							nonMatches[ret].datum = data[i][j];
						}
					}
				}

				break; //otherArchive.rowHeaders[otherI] has been matched and compared
			}
			else if (i == numRows - 1) //New row in otherArchive data array not present in this* Archive data array
			{
				for (int j = 0; j < numColumns; ++j)
				{
					if (bGetNonMatchSize)
						++ret;
					else if (nonMatches != nullptr)
					{
						--ret;
						nonMatches[ret].otherRowHeader = otherArchive.rowHeaders[otherI];
						nonMatches[ret].commonColumnHeader = columnHeaders[j];
						nonMatches[ret].otherDatum = otherArchive.data[otherI][j];
						nonMatches[ret].datum = wstring();
					}
				}
			}
		}
	}

	return ret;
}

bool Archive::bWriteArchiveFile(const wstring csvName)
{
	bool bSuccess = false;

	if (!csvName.empty())
		Archive::csvName = csvName;

	getOutArchive().open(Archive::csvName);

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

int Archive::fetchColumnIndex(const wstring columnHeader) const
{
	int columnIndex = -1;
	int j = 0;

	do
	{
		if (columnHeaders[j] == columnHeader)
			columnIndex = j;
		else
			++j;
	} while (columnIndex != j && j < numColumns);

	return columnIndex;
}

int Archive::getNumColumns() const
{
	return numColumns;
}

int Archive::getNumRows() const
{
	return numRows;
}

wstring Archive::getColumnHeader(const int j) const
{
	return columnHeaders[j];
}

wstring Archive::getRowHeader(const int i) const
{
	return rowHeaders[i];
}

wstring Archive::getDatum(const int i, const int j) const
{
	return data[i][j];
}

bool Archive::getBSuccessUseCsv() const
{
	return bSuccessUseCsv;
}

int Archive::getNumNonMatches() const
{
	return numNonMatches;
}

const Archive::NonMatch *const Archive::getNonMatches() const
{
	return nonMatches;
}

wstring Archive::getCsvName() const
{
	return csvName;
}

Archive::Archive()
{
	//Default values for when the default constructor is not called by the other delegating constructor
	numColumns = 0;
	numRows = 0;
	textFileOps = new TextFileOps;
	columnHeaders = nullptr;
	rowHeaders = nullptr;
	data = nullptr;
	inArchive = new wifstream;
	outArchive = new wofstream;
	bSuccessUseCsv = false;
	numNonMatches = 0;
	nonMatches = nullptr;
	csvName = wstring();
}

Archive::Archive(const wstring csvName) : Archive()
{
	numColumns = textFileOps->fetchNumColumns(getInArchive(), csvName) - 1;
	numRows = textFileOps->fetchNumRows(getInArchive(), csvName) - 1;
	columnHeaders = new wstring[numColumns];
	rowHeaders = new wstring[numRows];
	data = new wstring*[numRows];

	for (int i = 0; i < numRows; ++i)
		data[i] = new wstring[numColumns];

	bSuccessUseCsv = true; //Default to true because useCsv sets bSuccessUseCsv to false if there is an error
	Archive::csvName = csvName;
	useCsv();
}

Archive::Archive(const Archive &otherArchive, const void *const voidParam) : Archive()
{
	numColumns = otherArchive.numColumns;
	numRows = otherArchive.numRows;
	columnHeaders = new wstring[numColumns];

	for (int j = 0; j < numColumns; ++j)
		columnHeaders[j] = otherArchive.columnHeaders[j];

	rowHeaders = new wstring[numRows];
	data = new wstring*[numRows];

	for (int i = 0; i < numRows; ++i)
	{
		rowHeaders[i] = otherArchive.rowHeaders[i];
		data[i] = new wstring[numColumns];
	}
}

Archive::~Archive()
{
	delete textFileOps;

	if (columnHeaders != nullptr)
		delete[] columnHeaders;

	if (rowHeaders != nullptr)
		delete[] rowHeaders;

	if (data != nullptr)
	{
		for (int i = 0; i < numRows; ++i)
			delete[] data[i];

		delete[] data;
	}

	delete inArchive;
	delete outArchive;

	if (nonMatches != nullptr)
		delete[] nonMatches;
}

Archive &Archive::operator= (const Archive &otherArchive)
{
	if (this != &otherArchive) //Protect against invalid self-assignment
	{
		numColumns = otherArchive.numColumns;
		numRows = otherArchive.numRows;

		if (columnHeaders != nullptr)
			delete[] columnHeaders;

		columnHeaders = new wstring[numColumns];

		for (int j = 0; j < numColumns; ++j)
			columnHeaders[j] = otherArchive.columnHeaders[j];

		if (rowHeaders != nullptr)
			delete[] rowHeaders;

		rowHeaders = new wstring[numRows];

		if (data != nullptr)
		{
			for (int i = 0; i < numRows; ++i)
				delete[] data[i];

			delete[] data;
		}

		data = new wstring*[numRows];

		for (int i = 0; i < numRows; ++i)
		{
			rowHeaders[i] = otherArchive.rowHeaders[i];
			data[i] = new wstring[numColumns];

			for (int j = 0; j < numColumns; ++j)
				data[i][j] = otherArchive.data[i][j];
		}
	}

	return *this;
}

wifstream &Archive::getInArchive() const
{
	return *inArchive;
}

wofstream &Archive::getOutArchive() const
{
	return *outArchive;
}

void Archive::allocNonMatches(const Archive &otherArchive, int &ret)
{
	ret = compareArchives(otherArchive, true);

	if (ret > 0)
	{
		if (nonMatches != nullptr)
		{
			delete[] nonMatches;
			nonMatches = nullptr;
		}

		nonMatches = new NonMatch[ret];
		numNonMatches = ret;
	}
}

void Archive::useCsv()
{

	const bool sliceIsRow[2] = { false, true };
	const int numSlice[2] = { 1, 1 };
	const int sliceSize[2] = { numRows, numColumns };
	wstring *headers[2] = { rowHeaders, columnHeaders };

	for (int i = 0; i < 2; ++i)
	{
		if (textFileOps->fetchDelimitedSlice(getInArchive(), csvName, headers[i], sliceSize[i], sliceIsRow[i], 2, L',',
			numSlice[i]) != sliceSize[i]) //Fill rowHeaders and columnHeaders based on slices from the CSV file
		{
			bSuccessUseCsv = false;
			break;
		}
	}

	for (int i = 0; i < numRows; ++i) //Fill data array row-by-row from the CSV file
		textFileOps->fetchDelimitedSlice(getInArchive(), csvName, data[i], numColumns, true, 2, L',', i + 2);
}

void Archive::writeArchiveFileRow(const wstring newRow[]) const
{
	for (int j = 0; j < numColumns; ++j)
		getOutArchive() << L',' << newRow[j];
}
