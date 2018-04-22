#include "TextFileOps.h"
#include <fstream>
#include <string>

using namespace std;

int TextFileOps::fetchDelimitedSlice(wifstream &delimitedFile, const wstring filename, wstring parsedSlice[],
	const int maxElements, const bool bSliceIsRow, const int skipToElement, const wchar_t delimiter,
	const int numSlice) const
{
	int numElements = 0;
	int maxNumSlice;
	int targetNumElements;

	if (bSliceIsRow)
	{
		maxNumSlice = fetchNumRows(delimitedFile, filename, delimiter, skipToElement);
		targetNumElements = fetchNumColumns(delimitedFile, filename, delimiter, numSlice) - skipToElement + 1;
	}
	else
	{
		maxNumSlice = fetchNumColumns(delimitedFile, filename, delimiter, skipToElement);
		targetNumElements = fetchNumRows(delimitedFile, filename, delimiter, numSlice) - skipToElement + 1;
	}

	if (maxElements == 0) //Send max number of elements needed for parsedSlice when maxElements == 0
		numElements = targetNumElements;
	else if (numSlice <= maxNumSlice) //Check that the slice exists
	{
		delimitedFile.open(filename);

		if (!delimitedFile.fail())
		{
			wchar_t character;

			if (bSliceIsRow) //Skip to first element to parse when parsing a row
			{
				skipToRowNum(delimitedFile, character, numSlice);
				bSkipToColumnNum(delimitedFile, character, delimiter, skipToElement);
			}
			else //Skip to the desire row only when parsing a column
				skipToRowNum(delimitedFile, character, skipToElement);

			//Begin parsing characters for the slice elements to fill the slice in parsedSlice
			for (int i = 0; i < targetNumElements && i < maxElements; i++) //&& gives consistent behavior for blank end lines
			{
				int j;
				wchar_t parsedElement[_MAX_PATH];

				if (!bSliceIsRow) //Skip to desired column only when parsing a column
					bSkipToColumnNum(delimitedFile, character, delimiter, numSlice);

				for (j = 0; j < _MAX_PATH && !delimitedFile.eof(); ++j)
				{
					delimitedFile.get(character);

					if (character == delimiter || character == L'\n') //Store element characters until delimiter
						break;
					else
						parsedElement[j] = character;
				}

				parsedElement[j] = L'\0'; //Add terminal null character to character array
				parsedSlice[i] = parsedElement;
				++numElements;

				while (!bSliceIsRow && !delimitedFile.eof() && character != L'\n') //Skip to next row for parsing a column
					delimitedFile.get(character);
			}

			delimitedFile.close();
		}
	}

	return numElements;
}

int TextFileOps::fetchNumColumns(wifstream &delimitedFile, const wstring filename,
	const wchar_t delimiter, const int numRow) const
{
	int numColumns = 0;

	delimitedFile.open(filename);

	if (!delimitedFile.fail())
	{
		wchar_t character;

		skipToRowNum(delimitedFile, character, numRow);

		if (!delimitedFile.eof()) //Function returns 0 if end of file is reached before the row requested to enumerate columns
		{
			++numColumns; //Increment number of columns each time a delimiter is read until new line or end of file
			delimitedFile.get(character);

			while (!delimitedFile.eof() && character != L'\n')
			{
				if (character == delimiter)
					++numColumns;

				delimitedFile.get(character);
			}
		}

		delimitedFile.close();
	}

	return numColumns;
}

int TextFileOps::fetchNumRows(wifstream &delimitedFile, const wstring filename,
	const wchar_t delimiter, const int numColumn) const
{
	int numRows = 0;

	delimitedFile.open(filename);

	if (!delimitedFile.fail())
	{
		while (!delimitedFile.eof())
		{
			wchar_t character;

			if (bSkipToColumnNum(delimitedFile, character, delimiter, numColumn))
				break; //Column requested to enumerate rows for does not exist

			if (!delimitedFile.eof()) //Increment number of rows and skip to next row
			{
				delimitedFile.get(character);

				if (character != L'\n') //Skip blank rows
					++numRows;

				while (!delimitedFile.eof() && character != L'\n')
					delimitedFile.get(character);
			}
		}

		delimitedFile.close();
	}

	return numRows;
}

void TextFileOps::skipToRowNum(wifstream &delimitedFile, wchar_t &character, const int numRow) const
{
	for (int i = 1; i < numRow; ++i)
	{
		do
		{
			delimitedFile.get(character);
		} while (!delimitedFile.eof() && character != L'\n');
	}
}

bool TextFileOps::bSkipToColumnNum(wifstream &delimitedFile, wchar_t &character, const wchar_t delimiter, const int numColumn) const
{
	bool bTooFewColumns = false;

	for (int i = 1; i < numColumn; ++i)
	{
		do
		{
			delimitedFile.get(character);

			if (character == L'\n')
			{
				bTooFewColumns = true;
				break;
			}
		} while (!delimitedFile.eof() && character != delimiter);
	}

	return bTooFewColumns;
}

int TextFileOps::parseTextFile(const wstring searchTerm, wifstream &searchFile, wchar_t searchRes[][_MAX_PATH], const int maxRes,
	const wchar_t ignoreChars[], const int numIgnoreChars, const wchar_t retChar) const
{
	int instancesFound = 0;
	wstring testString;

	searchFile >> testString;

	while (!searchFile.eof() && instancesFound < maxRes)
	{
		if (testString == searchTerm)
		{
			wchar_t characterLast = L'\0';
			wchar_t character;
			int i = 0;

			searchFile.get(character);

			//Fill search result entry until new-line, end of file, or return character
			while (!searchFile.eof() && character != L'\n' && character != retChar)
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
				{
					searchRes[instancesFound][i] = character;
					++i;
				}

				characterLast = character;
				searchFile.get(character);
			}

			searchRes[instancesFound][i] = L'\0'; //Add terminal null character to character array
			++instancesFound;

			if (character == retChar)
				break;
		}
		else
			searchFile >> testString;
	}

	return instancesFound;
}
