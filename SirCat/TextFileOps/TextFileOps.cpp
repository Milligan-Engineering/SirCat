#ifndef STRICT
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "TextFileOps.h"
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

int TextFileOps::fetchDelimitedSlice(wifstream &delimitedFile, const wstring filename, wstring parsedSlice[],
	const int maxElements, const bool bSliceIsRow, const int skipToElement, const WCHAR delimiter,
	const int numSlice)
{
	int numElements = 0;
	WCHAR character;
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
				WCHAR parsedElement[MAX_PATH];

				if (!bSliceIsRow) //Skip to desired column only when parsing a column
					bSkipToColumnNum(delimitedFile, character, delimiter, numSlice);

				for (j = 0; j < MAX_PATH && !delimitedFile.eof(); ++j)
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

int TextFileOps::fetchNumColumns(wifstream &delimitedFile, const wstring filename, const WCHAR delimiter, const int numRow)
{
	int numColumns = 0;
	WCHAR character;

	delimitedFile.open(filename);

	if (!delimitedFile.fail())
	{
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

int TextFileOps::fetchNumRows(wifstream &delimitedFile, const wstring filename, const WCHAR delimiter, const int numColumn)
{
	int numRows = 0;
	WCHAR character;
	bool bTooFewColumns = false;

	delimitedFile.open(filename);

	if (!delimitedFile.fail())
	{
		while (!delimitedFile.eof())
		{
			if (bTooFewColumns = bSkipToColumnNum(delimitedFile, character, delimiter, numColumn)) //Single = is intentional
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

void TextFileOps::skipToRowNum(wifstream &delimitedFile, WCHAR &character, const int numRow)
{
	for (int i = 1; i < numRow; ++i)
	{
		do
		{
			delimitedFile.get(character);
		} while (!delimitedFile.eof() && character != L'\n');
	}
}

bool TextFileOps::bSkipToColumnNum(wifstream &delimitedFile, WCHAR &character, const WCHAR delimiter, const int numColumn)
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

int TextFileOps::parseTextFile(const wstring searchTerm, wifstream &searchFile, WCHAR searchRes[][MAX_PATH], const int maxRes,
	const WCHAR ignoreChars[], const int numIgnoreChars, const WCHAR retChar)
{
	int instancesFound = 0;
	wstring testString;
	WCHAR character;

	searchFile >> testString;

	while (!searchFile.eof() && instancesFound < maxRes)
	{
		if (testString == searchTerm)
		{
			WCHAR characterLast = L'\0';
			int i = 0;

			searchFile.get(character);

			//Fill search result entry until new-line, end of file, or return character
			while (!searchFile.eof() && character != L'\n' && character != retChar)
			{
				bool bIgnoreChar = false;

				for (int j = 0; j < numIgnoreChars; ++j)
				{
					if (character == ignoreChars[j]
						|| (characterLast == L'\\' && character == L'\\')) //Detect escaped backslashes
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
