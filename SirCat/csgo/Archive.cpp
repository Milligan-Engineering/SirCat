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

Archive::Archive(const wstring csvName) : GameData(), bSuccessUseCsv(true), csvName(csvName)
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
	
	useCsv(params); //Sets bSuccessUseCsv to false if there is an error
}

void Archive::useCsv(TextFileOps::Params &params)
{
	const bool k_b_slice_is_row[2] = { false, true };
	const int k_num_slice[2] = { 1, 1 };
	const int k_slice_size[2] = { numRows, numColumns };
	wstring *headers[2] = { rowHeaders, columnHeaders };

	params.skipToElement = 2;

	for (int i = 0; i < 2; ++i)
	{
		params.parsedSlice = headers[i];
		params.maxElements = k_slice_size[i];
		params.bSliceIsRow = k_b_slice_is_row[i];
		params.numSlice = k_num_slice[i];

		if (textFileOps->fetchDelimitedSlice(params) != k_slice_size[i]) //Fills rowHeaders and columnHeaders from CSV file slices
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
