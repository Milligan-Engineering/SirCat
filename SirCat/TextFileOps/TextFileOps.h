#pragma once

#include <iosfwd>
#include <string>

using namespace std;

class TextFileOps
{
public:
	TextFileOps() = default;
	~TextFileOps() = default;

	int fetchDelimitedSlice(wifstream &delimitedFile, const wstring filename, wstring parsedSlice[] = nullptr,
		const int maxElements = 0, const bool bSliceIsRow = true, const int skipToElement = 1, const wchar_t delimiter = L',',
		const int numSlice = 1) const;
	//Precondition: 
	//Postcondition: 

	int fetchNumColumns(wifstream &delimitedFile, const wstring filename,
		const wchar_t delimiter = L',', const int numRow = 1) const;
	//Precondition: 
	//Postcondition: 

	int fetchNumRows(wifstream &delimitedFile, const wstring filename,
		const wchar_t delimiter = L',', const int numColumn = 1) const;
	//Precondition: 
	//Postcondition: 

	void skipToRowNum(wifstream &delimitedFile, wchar_t &character, const int numRow) const;
	//Precondition: 
	//Postcondition: 

	bool bSkipToColumnNum(wifstream &delimitedFile, wchar_t &character, const wchar_t delimiter,
		const int numColumn) const;
	//Precondition: 
	//Postcondition: 

	int parseTextFile(const wstring searchTerm, wifstream &searchFile, wchar_t searchRes[][_MAX_PATH], const int maxRes,
		const wchar_t ignoreChars[] = L"", const int numIgnoreChars = 0, const wchar_t retChar = L'\0') const;
	//Precondition: searchTerm should not contain any whitespace characters.
		//The file input stream searchFile has been successfully connected to a file.
		//The two-dimensional array searchRes is modifiable.
		//maxRes is less than or equal to the first dimension in searchRes.
		//If the array ignoreChars is passed, numIgnoreChars is less than or equal to the size of ignoreChars.
	//Postcondition: searchFile is searched by looping through non-whitespace strings separated by whitespace characters.
		//When a string matches searchTerm, a *searchRes[MAX_PATH] array is filled until new-line or end of file.
		//Characters passed in ignoreChars are not copied to *searchRes[MAX_PATH] and escaped backslashes are unescaped.
		//searchRes is filled, up to maxRes number of null-terminated wstrings *searchRes[MAX_PATH].
		//If retChar is passed, the function will return if and when that character is found.
		//Returns the number of null-terminated wstrings *searchRes[MAX_PATH] filled in the array of wstrings searchRes.
private:
	bool bPrepForSlicing(wifstream &delimitedFile, const wstring filename, const int maxElements,
		const bool bSliceIsRow, const int skipToElement, const wchar_t delimiter, const int numSlice,
		int &targetNumElements, int &numElements) const;
	//Precondition: 
	//Postcondition: 
};
