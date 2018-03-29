#include "TextFileOps.h"
#include <fstream>
#include <string>

TextFileOps &TextFileOps::inst()
{
	static TextFileOps inst;
	return inst;
}

int TextFileOps::fetchDelimitedSlice(ifstream &delimitedFile, const string k_filename, string parsedSlice[],
	const int k_max_elements, const bool bSliceIsRow, const int k_skip_to_element, const char k_delimiter, const int k_num_slice)
{
	int numElements = 0;
	char character;
	int maxNumSlice;
	int targetNumElements;

	if (bSliceIsRow)
	{
		maxNumSlice = fetchNumRows(delimitedFile, k_filename, k_delimiter, k_skip_to_element);
		targetNumElements = fetchNumColumns(delimitedFile, k_filename, k_delimiter, k_num_slice) - k_skip_to_element + 1;
	}
	else
	{
		maxNumSlice = fetchNumColumns(delimitedFile, k_filename, k_delimiter, k_skip_to_element);
		targetNumElements = fetchNumRows(delimitedFile, k_filename, k_delimiter, k_num_slice) - k_skip_to_element + 1;
	}

	if (k_num_slice <= maxNumSlice) //Check that the slice exists
	{
		delimitedFile.open(k_filename);

		if (!delimitedFile.fail())
		{
			if (bSliceIsRow) //Skip to first element to parse when parsing a row
			{
				skipToRowNum(delimitedFile, character, k_num_slice);
				bSkipToColumnNum(delimitedFile, character, k_delimiter, k_skip_to_element);
			}
			else //Skip to the desire row only when parsing a column
				skipToRowNum(delimitedFile, character, k_skip_to_element);

			//Begin parsing characters for the slice elements to fill the slice in parsedSlice
			for (int i = 0; i < targetNumElements && i < k_max_elements; i++) //AND gives consistent behavior for blank end lines
			{
				int j;
				char parsedElement[k_max_path];

				if (!bSliceIsRow) //Skip to desired column only when parsing a column
					bSkipToColumnNum(delimitedFile, character, k_delimiter, k_num_slice);

				for (j = 0; j < k_max_path && !delimitedFile.eof(); ++j)
				{
					delimitedFile.get(character);

					if (character == k_delimiter || character == '\n') //Store element characters until delimiter
						break;
					else
						parsedElement[j] = character;
				}

				parsedElement[j] = '\0'; //Add terminal null character to character array
				parsedSlice[i] = static_cast<string>(parsedElement); //Fill element with character array
				++numElements;

				while (!bSliceIsRow && !delimitedFile.eof() && character != '\n') //Skip to next row for parsing a column
					delimitedFile.get(character);
			}

			delimitedFile.close();
		}
	}

	return numElements;
}

int TextFileOps::fetchNumColumns(ifstream &delimitedFile, const string k_filename, const char k_delimiter, const int k_num_row)
{
	int numColumns = 0;
	char character;

	delimitedFile.open(k_filename);

	if (!delimitedFile.fail())
	{
		skipToRowNum(delimitedFile, character, k_num_row);

		if (!delimitedFile.eof()) //Function returns 0 if end of file is reached before the row requested to enumerate columns
		{
			++numColumns; //Increment number of columns each time a delimiter is read until new line or end of file
			delimitedFile.get(character);

			while (!delimitedFile.eof() && character != '\n')
			{
				if (character == k_delimiter)
					++numColumns;

				delimitedFile.get(character);
			}
		}

		delimitedFile.close();
	}

	return numColumns;
}

int TextFileOps::fetchNumRows(ifstream &delimitedFile, const string k_filename, const char k_delimiter, const int k_num_column)
{
	int numRows = 0;
	char character;
	bool bTooFewColumns = false;

	delimitedFile.open(k_filename);

	if (!delimitedFile.fail())
	{
		while (!delimitedFile.eof())
		{
			if (bTooFewColumns = bSkipToColumnNum(delimitedFile, character, k_delimiter, k_num_column)) //Single = is intentional
				break; //Column requested to enumerate rows for does not exist and function will return 0

			if (!delimitedFile.eof()) //Increment number of rows and skip to next row until end of file
			{
				++numRows;

				do
				{
					delimitedFile.get(character);
				} while (!delimitedFile.eof() && character != '\n');
			}
		}

		delimitedFile.close();
	}

	return numRows;
}

void TextFileOps::skipToRowNum(ifstream &delimitedFile, char &character, const int k_num_row)
{
	for (int i = 1; i < k_num_row; ++i)
	{
		do
		{
			delimitedFile.get(character);
		} while (!delimitedFile.eof() && character != '\n');
	}
}

bool TextFileOps::bSkipToColumnNum(ifstream &delimitedFile, char &character, const char k_delimiter, const int k_num_column)
{
	bool bTooFewColumns = false;

	for (int i = 1; i < k_num_column; ++i)
	{
		do
		{
			delimitedFile.get(character);

			if (character == '\n')
			{
				bTooFewColumns = true;
				break;
			}
		} while (!delimitedFile.eof() && character != k_delimiter);
	}

	return bTooFewColumns;
}

int TextFileOps::concatCharArrays(char cArray1[], char cArray2[], char concatArray[], const int k_size_array)
{
	int concatIndex = 0;

	for (int i = 0; cArray1[i] != '\0' && concatIndex < k_size_array - 1; ++i)
	{
		concatArray[concatIndex] = cArray1[i];
		++concatIndex;
	}

	for (int i = 0; cArray2[i] != '\0' && concatIndex < k_size_array - 1; ++i)
	{
		concatArray[concatIndex] = cArray2[i];
		++concatIndex;
	}

	concatArray[concatIndex] = '\0';

	return concatIndex;
}

int TextFileOps::parseTextFile(const string searchTerm, ifstream &searchFile, char searchRes[][k_max_path], const int k_max_res,
	const char k_ignore_chars[], const int k_num_ignore_chars, const char k_ret_char)
{
	int instancesFound = 0;
	string testString;
	char character;

	searchFile >> testString;

	while (!searchFile.eof() && instancesFound < k_max_res)
	{
		if (testString == searchTerm)
		{
			char characterLast = '\0';
			int i = 0;

			searchFile.get(character);

			//Fill search result entry until new-line or end of file
			while (!searchFile.eof() && character != '\n' && character != k_ret_char)
			{
				bool bIgnoreChar = false;

				for (int j = 0; j < k_num_ignore_chars; ++j)
				{
					if (character == k_ignore_chars[j]
						|| (characterLast == '\\' && character == '\\')) //Detect escaped backslashes
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

			searchRes[instancesFound][i] = '\0'; //Add terminal null character to character array
			++instancesFound;

			if (character == k_ret_char)
				break;
		}
		else
			searchFile >> testString;
	}

	return instancesFound;
}
