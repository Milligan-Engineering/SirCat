#include "GameData.h"

#include "..\util\TextFileOps.h"
#include <fstream>
#include <ostream>
#include <string>

namespace sircat {
namespace csgo {

using sircat::util::TextFileOps;
using std::wofstream;
using std::endl;
using std::wstring;

GameData::~GameData()
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

	delete outCsv;
	deleteNonMatches();
}

GameData &GameData::operator= (const GameData &otherGameData)
{
	if (this != &otherGameData) //No self-assignment
	{
		numColumns = otherGameData.numColumns;
		numRows = otherGameData.numRows;

		if (columnHeaders != nullptr)
			delete[] columnHeaders;

		columnHeaders = new wstring[numColumns];

		for (int j = 0; j < numColumns; ++j)
			columnHeaders[j] = otherGameData.columnHeaders[j];

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
			rowHeaders[i] = otherGameData.rowHeaders[i];
			data[i] = new wstring[numColumns];

			for (int j = 0; j < numColumns; ++j)
				data[i][j] = otherGameData.data[i][j];
		}

		deleteNonMatches();
	}

	return *this;
}

int GameData::compareGameData(const GameData &otherGameData, const bool bGetNonMatchSize)
{
	int ret = 0;

	if (!bGetNonMatchSize)
		allocNonMatches(otherGameData, ret);

	for (int otherI = 0; otherI < otherGameData.numRows; ++otherI) //Index for first dimension of otherArchive data array
	{
		for (int i = 0; i < numRows; ++i) //Index for first dimension of this* Archive data array
		{
			if (otherGameData.rowHeaders[otherI] == rowHeaders[i])
			{
				for (int j = 0; j < numColumns; ++j) //Index for second dimension of both data arrays
				{
					if (otherGameData.data[otherI][j] != data[i][j])
					{
						if (bGetNonMatchSize)
							++ret;
						else if (nonMatches != nullptr)
						{
							--ret;
							nonMatches[ret].otherRowHeader = otherGameData.rowHeaders[otherI];
							nonMatches[ret].commonColumnHeader = columnHeaders[j];
							nonMatches[ret].otherDatum = otherGameData.data[otherI][j];
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
						nonMatches[ret].otherRowHeader = otherGameData.rowHeaders[otherI];
						nonMatches[ret].commonColumnHeader = columnHeaders[j];
						nonMatches[ret].otherDatum = otherGameData.data[otherI][j];
						nonMatches[ret].datum = wstring();
					}
				}
			}
		}
	}

	return ret;
}

void GameData::deleteNonMatches()
{
	if (nonMatches != nullptr)
	{
		delete[] nonMatches;
		nonMatches = nullptr;
	}

	numNonMatches = 0;
}

bool GameData::bWriteCsvFile(const wstring csvName)
{
	bool bSuccess = false;

	getOutCsv().open(csvName);

	if (!getOutCsv().fail())
	{
		writeCsvFileRow(columnHeaders);
		getOutCsv() << endl;

		for (int i = 0; i < numRows; ++i)
		{
			getOutCsv() << rowHeaders[i];
			writeCsvFileRow(data[i]);
			getOutCsv() << endl;
		}

		bSuccess = true;
		getOutCsv().close();
	}

	return bSuccess;
}

int GameData::fetchColumnIndex(const wstring columnHeader) const
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

int GameData::getNumColumns() const
{
	return numColumns;
}

int GameData::getNumRows() const
{
	return numRows;
}

wstring GameData::getColumnHeader(const int j) const
{
	return columnHeaders[j];
}

wstring GameData::getRowHeader(const int i) const
{
	return rowHeaders[i];
}

wstring GameData::getDatum(const int i, const int j) const
{
	return data[i][j];
}

int GameData::getNumNonMatches() const
{
	return numNonMatches;
}

const GameData::NonMatch *const GameData::getNonMatches() const
{
	return nonMatches;
}

GameData::GameData() noexcept : numColumns(0), numRows(0), textFileOps(new TextFileOps), columnHeaders(nullptr),
								rowHeaders(nullptr), data(nullptr), outCsv(new wofstream), numNonMatches(0), nonMatches(nullptr)
{}

GameData::GameData(const GameData &otherGameData, const void *const) : GameData()
{
	numColumns = otherGameData.numColumns;
	numRows = otherGameData.numRows;
	columnHeaders = new wstring[numColumns];

	for (int j = 0; j < numColumns; ++j)
		columnHeaders[j] = otherGameData.columnHeaders[j];

	rowHeaders = new wstring[numRows];
	data = new wstring*[numRows];

	for (int i = 0; i < numRows; ++i)
	{
		rowHeaders[i] = otherGameData.rowHeaders[i];
		data[i] = new wstring[numColumns];
	}
}

void GameData::allocNonMatches(const GameData &otherGameData, int &ret)
{
	ret = compareGameData(otherGameData, true);

	if (ret > 0)
	{
		deleteNonMatches();
		nonMatches = new NonMatch[ret];
		numNonMatches = ret;
	}
}

void GameData::writeCsvFileRow(const wstring newRow[]) const
{
	for (int j = 0; j < numColumns; ++j)
		getOutCsv() << L',' << newRow[j];
}

wofstream &GameData::getOutCsv() const
{
	return *outCsv;
}

} //namespace csgo
} //namespace sircat
