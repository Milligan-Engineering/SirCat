#include "TextFileOps.h"

#include <cstdlib>
#include <fstream>
#include <string>

namespace sircat {
namespace util {

using std::wifstream;
using std::wstring;

int TextFileOps::fetchDelimitedSlice(Params &params) const
{
	int numElements = 0;

	int targetNumElements;

	if (bPrepForSlicing(numElements, targetNumElements, params))
	{
		int i = 0;

		while (i < targetNumElements && i < params.maxElements && !params.delimitedFile->eof())
		{
			int j = 0;
			wchar_t parsedElement[_MAX_PATH];

			if (!params.bSliceIsRow) //Skip to desired column only when parsing a column
				bSkipToColumnNum(params.numSlice, params);

			params.delimitedFile->get(params.wchar);

			while (j < _MAX_PATH && !params.delimitedFile->eof() && params.wchar != params.delimiter && params.wchar != L'\n')
			{
				parsedElement[j] = params.wchar; //Store element characters until delimiter
				params.delimitedFile->get(params.wchar);
				++j;
			}

			parsedElement[j] = L'\0'; //Add terminal null character to character array
			params.parsedSlice[i] = parsedElement;
			++numElements;
			++i;

			while (!params.bSliceIsRow && !params.delimitedFile->eof() && params.wchar != L'\n')
				params.delimitedFile->get(params.wchar); //Skip to next row for parsing a column
		}

		params.delimitedFile->close();
	}

	return numElements;
}

int TextFileOps::fetchNumColumns(Params &params, const int numRow) const
{
	int numColumns = 0;

	params.delimitedFile->open(params.filename);

	if (!params.delimitedFile->fail())
	{
		skipToRowNum(numRow, params);

		if (!params.delimitedFile->eof()) //Increments number of columns each time delimiter read until new line or end of file
		{
			++numColumns;
			params.delimitedFile->get(params.wchar);

			while (!params.delimitedFile->eof() && params.wchar != L'\n')
			{
				if (params.wchar == params.delimiter)
					++numColumns;

				params.delimitedFile->get(params.wchar);
			}
		}

		params.delimitedFile->close();
	}

	return numColumns;
}

int TextFileOps::fetchNumRows(Params &params, const int numColumn) const
{
	int numRows = 0;

	params.delimitedFile->open(params.filename);

	if (!params.delimitedFile->fail())
	{
		while (!bSkipToColumnNum(numColumn, params) && !params.delimitedFile->eof())
		{
			params.delimitedFile->get(params.wchar);

			if (params.wchar != L'\n') //Skip blank rows
				++numRows; //Increment number of rows

			while (!params.delimitedFile->eof() && params.wchar != L'\n')
				params.delimitedFile->get(params.wchar); //Skip to next row
		}

		params.delimitedFile->close();
	}

	return numRows;
}

void TextFileOps::skipToRowNum(const int numRow, Params &params) const
{
	for (int currentRow = 1; currentRow < numRow; ++currentRow)
	{
		do
		{
			params.delimitedFile->get(params.wchar);
		} while (!params.delimitedFile->eof() && params.wchar != L'\n');
	}
}

bool TextFileOps::bSkipToColumnNum(const int numColumn, Params &params) const
{
	bool bTooFewColumns = false;

	int currentColumn = 1;

	while (currentColumn < numColumn && bTooFewColumns == false)
	{
		do //Skip to next delimiter
		{
			params.delimitedFile->get(params.wchar);

			if (params.wchar == L'\n')
				bTooFewColumns = true;
		} while (!params.delimitedFile->eof() && params.wchar != params.delimiter && bTooFewColumns == false);

		++currentColumn;
	}

	return bTooFewColumns;
}

int TextFileOps::parseTextFile(const wstring searchTerm, wifstream &file, wstring searchRes[], const int maxRes,
							   const wchar_t ignoreChars[], const int numIgnoreChars, const wchar_t retChar) const
{
	int instancesFound = 0;

	wchar_t character = L'\b'; //Backspace character is arbitrarily used to allow the comparison in the first while statement
	wstring testString;

	file >> testString;

	while (!file.eof() && instancesFound < maxRes && character != retChar)
	{
		if (testString == searchTerm)
		{
			wchar_t characterLast = L'\0';

			file.get(character);

			while (!file.eof() && character != L'\n' && character != retChar
				   && searchRes[instancesFound].size() < searchRes[instancesFound].max_size()) //Fills search result entry
			{
				bool bIgnoreChar = false;

				for (int j = 0; j < numIgnoreChars; ++j)
				{
					if (character == ignoreChars[j] || (characterLast == L'\\' && character == L'\\')) //For escaped backslashes
					{
						bIgnoreChar = true;
						break;
					}
				}

				if (!bIgnoreChar)
					searchRes[instancesFound] += character;

				characterLast = character;
				file.get(character);
			}

			++instancesFound;
		}
		else
			file >> testString;
	}

	return instancesFound;
}

bool TextFileOps::bPrepForSlicing(int &numElements, int &targetNumElements, Params &params) const
{
	bool bSuccess = false;

	int maxNumSlice;

	if (params.bSliceIsRow)
	{
		maxNumSlice = fetchNumRows(params, params.skipToElement);
		targetNumElements = fetchNumColumns(params, params.numSlice) - params.skipToElement + 1;
	}
	else
	{
		maxNumSlice = fetchNumColumns(params, params.skipToElement);
		targetNumElements = fetchNumRows(params, params.numSlice) - params.skipToElement + 1;
	}

	if (params.maxElements == 0) //Returns max number of elements needed for parsedSlice when maxElements == 0
		numElements = targetNumElements;
	else if (params.numSlice <= maxNumSlice) //Checks that the slice exists
	{
		params.delimitedFile->open(params.filename);

		if (!params.delimitedFile->fail())
		{
			if (params.bSliceIsRow) //Skips to first element to parse when parsing a row
			{
				skipToRowNum(params.numSlice, params);
				bSkipToColumnNum(params.skipToElement, params);
			}
			else //Skips to the desired row only when parsing a column
				skipToRowNum(params.skipToElement, params);

			bSuccess = true;
		}
	}

	return bSuccess;
}

} //namespace util
} //namespace sircat
