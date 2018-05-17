#include "Archive.h"
#include "..\util\TextFileOps.h"
#include <fstream>
#include <string>

namespace sircat {
namespace csgo {

using sircat::util::TextFileOps;
using std::wifstream;
using std::wstring;

bool Archive::bWriteArchiveFile()
{
	return bWriteCsvFile(csvName);
}

bool Archive::getBSuccessUseCsv() const
{
	return bSuccessUseCsv;
}

wstring Archive::getCsvName() const
{
	return csvName;
}

Archive::Archive(const wstring csvName) : GameData()
{
	wifstream inArchive;
	TextFileOps::Params params;

	params.delimitedFile = &inArchive;
	params.filename = csvName;
	numColumns = textFileOps->fetchNumColumns(params) - 1;
	numRows = textFileOps->fetchNumRows(params) - 1;

	columnHeaders = new wstring[numColumns];
	rowHeaders = new wstring[numRows];
	data = new wstring*[numRows];

	for (int i = 0; i < numRows; ++i)
		data[i] = new wstring[numColumns];

	bSuccessUseCsv = true; //Defaults to true because useCsv sets bSuccessUseCsv to false if there is an error
	Archive::csvName = csvName;
	useCsv(params);
}

void Archive::useCsv(TextFileOps::Params &params)
{
	const bool sliceIsRow[2] = { false, true };
	const int numSlice[2] = { 1, 1 };
	const int sliceSize[2] = { numRows, numColumns };
	wstring *headers[2] = { rowHeaders, columnHeaders };

	params.skipToElement = 2;

	for (int i = 0; i < 2; ++i)
	{
		params.parsedSlice = headers[i];
		params.maxElements = sliceSize[i];
		params.bSliceIsRow = sliceIsRow[i];
		params.numSlice = numSlice[i];

		if (textFileOps->fetchDelimitedSlice(params) != sliceSize[i]) //Fills rowHeaders and columnHeaders from CSV file slices
		{
			bSuccessUseCsv = false;
			break;
		}
	}

	if (bSuccessUseCsv)
	{
		for (int i = 0; i < numRows; ++i) //Fills data array row-by-row from the CSV file
		{
			params.parsedSlice = data[i];
			params.maxElements = numColumns;
			params.numSlice = i + 2;

			textFileOps->fetchDelimitedSlice(params);
		}
	}
}

} //namespace csgo
} //namespace sircat
