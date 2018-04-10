#pragma once

#ifndef STRICT
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <iosfwd>
#include <string>
#include <Windows.h>

using namespace std;

class TextFileOps
{
public:
	static TextFileOps &inst();
	//Precondition: 
	//Postcondition: 

	int fetchDelimitedSlice(wifstream &delimitedFile, const wstring filename, wstring parsedSlice[],
		const int maxElements, const bool bSliceIsRow = true, const int skipToElement = 1, const WCHAR delimiter = L',',
		const int numSlice = 1);
	//Precondition: 
	//Postcondition: 

	int fetchNumColumns(wifstream &delimitedFile, const wstring filename, const WCHAR delimiter = L',', const int numRow = 1);
	//Precondition: 
	//Postcondition: 

	int fetchNumRows(wifstream &delimitedFile, const wstring filename, const WCHAR delimiter = L',', const int numColumn = 1);
	//Precondition: 
	//Postcondition: 

	void skipToRowNum(wifstream &delimitedFile, WCHAR &character, const int numRow);
	//Precondition: 
	//Postcondition: 

	bool bSkipToColumnNum(wifstream &delimitedFile, WCHAR &character, const WCHAR delimiter, const int numColumn);
	//Precondition: 
	//Postcondition: 

	int parseTextFile(const wstring searchTerm, wifstream &searchFile, WCHAR searchRes[][MAX_PATH], const int maxRes,
		const WCHAR ignoreChars[] = L"", const int numIgnoreChars = 0, const WCHAR retChar = L'\0');
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
	TextFileOps() = default;
	~TextFileOps() = default;
};
