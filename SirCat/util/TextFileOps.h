#pragma once

#include <fstream>
#include <string>
#include <stdlib.h>

namespace sircat {
namespace util {

class TextFileOps
{
public:
	struct Params
	{
		std::wifstream *delimitedFile;
		std::wstring filename;
		std::wstring *parsedSlice;
		int maxElements;
		bool bSliceIsRow;
		int skipToElement;
		wchar_t delimiter;
		int numSlice;
		wchar_t wchar;

		Params() : parsedSlice(nullptr), maxElements(0), bSliceIsRow(true), skipToElement(1), delimiter(L','), numSlice(1) {};
	};

	TextFileOps() = default;
	~TextFileOps() = default;

	int fetchDelimitedSlice(Params &params) const;

	int fetchNumColumns(Params &params, const int numRow = 1) const;
	//Postcondition: Function returns 0 if end of file is reached before the row requested to enumerate columns

	int fetchNumRows(Params &params, const int numColumn = 1) const;

	void skipToRowNum(const int numRow, Params &params) const;

	bool bSkipToColumnNum(const int numColumn, Params &params) const;

	int parseTextFile(const std::wstring searchTerm, std::wifstream &file, wchar_t searchRes[][_MAX_PATH], const int maxRes,
					  const wchar_t ignoreChars[] = L"", const int numIgnoreChars = 0, const wchar_t retChar = L'\0') const;
	//Precondition: searchTerm should not contain any whitespace characters
		//The file input stream searchFile has been successfully connected to a file
		//The two-dimensional array searchRes is modifiable
		//maxRes is less than or equal to the first dimension in searchRes
		//If the array ignoreChars is passed, numIgnoreChars is less than or equal to the size of ignoreChars
	//Postcondition: searchFile is searched by looping through non-whitespace strings separated by whitespace characters
		//When a string matches searchTerm, a *searchRes[MAX_PATH] array is filled until new-line or end of file
		//Characters passed in ignoreChars are not copied to *searchRes[MAX_PATH] and escaped backslashes are unescaped
		//searchRes is filled, up to maxRes number of null-terminated wstrings *searchRes[MAX_PATH]
		//If retChar is passed, the function will return if and when that character is found
		//Returns the number of null-terminated wstrings *searchRes[MAX_PATH] filled in the array of wstrings searchRes
private:
	bool bPrepForSlicing(int &numElements, int &targetNumElements, Params &params) const;
};

} //namespace util
} //namespace sircat
