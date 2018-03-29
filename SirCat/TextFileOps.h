#pragma once

#include <iosfwd> //Forward declares ifstream without #include <fstream>
#include <string>

using namespace std;

class TextFileOps
{
public:
	static const int k_max_path = 260;

	static TextFileOps &inst();
	//Precondition: 
	//Postcondition: 

	int fetchDelimitedSlice(ifstream &delimitedFile, const string k_filename, string parsedSlice[], const int k_max_elements,
		const bool bSliceIsRow = true, const int k_skip_to_element = 1, const char k_delimiter = ',', const int k_num_slice = 1);
	//Precondition: 
	//Postcondition: 

	int fetchNumColumns(ifstream &delimitedFile, const string k_filename, const char k_delimiter = ',', const int k_num_row = 1);
	//Precondition: 
	//Postcondition: 

	int fetchNumRows(ifstream &delimitedFile, const string k_filename, const char k_delimiter = ',', const int k_num_column = 1);
	//Precondition: 
	//Postcondition: 

	void skipToRowNum(ifstream &delimitedFile, char &character, const int k_num_row);
	//Precondition: 
	//Postcondition: 

	bool bSkipToColumnNum(ifstream &delimitedFile, char &character, const char k_delimiter, const int k_num_column);
	//Precondition: 
	//Postcondition: 

	int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], const int k_size_array);
	//Precondition: The strings cArray1 and cArray2 are null-terminated.
		//The sum of filled elements of cArray1 and cArray2 is less than or equal to the size of array concatArray.
		//concatArray is modifiable.
		//k_size_array is the size of concatArray.
	//Postcondition: concatArray is filled with the concatenation of cArray1 and cArray2, and terminated with a null character.
		//cArray1 or cArray2 can be the same array as concatArray to append or prepend to the original string.
		//Returns the index of the last filled element in concatArray.

	int parseTextFile(const string searchTerm, ifstream &searchFile, char searchRes[][k_max_path], const int k_max_res,
		const char k_ignore_chars[] = "", const int k_num_ignore_chars = 0, const char k_ret_char = '\0');
	//Precondition: searchTerm should not contain any whitespace characters.
		//The file input stream searchFile has been successfully connected to a file.
		//The two-dimensional array searchRes is modifiable.
		//k_max_res is less than or equal to the first dimension in searchRes.
		//If the array k_ignore_chars is passed, k_num_ignore_chars is less than or equal to the size of k_ignore_chars.
	//Postcondition: searchFile is searched by looping through non-whitespace strings separated by whitespace characters.
		//When a string matches searchTerm, a *searchRes[k_max_path] array is filled until new-line or end of file.
		//Characters passed in k_ignore_chars are not copied to *searchRes[k_max_path] and escaped backslashes are unescaped.
		//searchRes is filled, up to k_max_res number of null-terminated strings *searchRes[k_max_path].
		//If k_ret_char is passed, the function will return if and when that character is found.
		//Returns the number of null-terminated strings *searchRes[k_max_path] filled in the array of strings searchRes.
private:
	TextFileOps() = default;
	~TextFileOps() = default;
};
