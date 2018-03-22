//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 16, 2018

#include "stdafx.h"

using namespace std;

bool bUserMenu(int &menuOption);
//Precondition: menuOption is modifiable.
//Postcondition: menuOption is updated according to user input in response to a program menu.
	//Returns true if the program should continue or false if the program should exit.

int takeOnlyOneInt(const char validChars[], const int numValidChars);
//Precondition: Array validChars is modifiable and numValidChars is the size of validChars.
//Postcondition: Get one and only one input char, convert it to int, and return the integer;

bool bTakeOnlyOneChar(char &character);
//Precondition: character is modifiable.
//Postcondition: Sets character to first character input 

int charDigitToInt(char charDigit);
//Precondition: 
//Postcondition: 

char intDigitToChar(int intDigit);
//Precondition: 
//Postcondition:

bool bIsDigit(char character);
//Precondition: 
//Postcondition: 

void hitEnterToExit();
//Precondition: 
//Postcondition: 

int getDelimitedSlice(ifstream &delimitedFile, const char k_filename[], string parsedSlice[], const int k_max_elements,
	const bool bSliceIsRow = true, const int k_skip_to_element = 1, const char k_delimiter = ',', const int k_num_slice = 1);
//Precondition: 
//Postcondition: 

int getNumColumns(ifstream &delimitedFile, const char k_filename[], const char k_delimiter = ',', const int k_num_row = 1);
//Precondition: 
//Postcondition: 

int getNumRows(ifstream &delimitedFile, const char k_filename[], const char k_delimiter = ',', const int k_num_column = 1);
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

int parseTextFile(string searchTerm, ifstream &searchFile, char searchResults[][k_max_path], const int k_max_results,
                  const char k_ignore_chars[] = "", const int k_num_ignore_chars = 0, const char k_ret_char = '\0');
//Precondition: searchTerm should not contain any whitespace characters.
	//The file input stream searchFile has been successfully connected to a file.
	//The two-dimensional array searchResults is modifiable.
	//k_max_results is less than or equal to the first dimension in searchResults.
	//If the array k_ignore_chars is passed, k_num_ignore_chars is less than or equal to the size of k_ignore_chars.
//Postcondition: searchFile is searched by looping through non-whitespace strings that are separated by whitespace characters.
	//When a string matches searchTerm, a *searchResults[k_max_path] array is filled until new-line or end of file.
	//Characters passed in k_ignore_chars are not copied to *searchResults[k_max_path] and escaped backslashes are unescaped.
	//searchResults is filled, up to k_max_results number of null-terminated strings *searchResults[k_max_path].
	//If k_ret_char is passed, the function will return if and when that character is found.
	//Returns the number of null-terminated strings *searchResults[k_max_path] filled in the array of strings searchResults.

bool bGetSteamDir(char steamDir[k_max_path]);
//Precondition: The array steamDir is modifiable.
//Postcondition: If a valid registry value for a Steam installation is found, steamDir is filled with the installation directory.
//Returns true if a valid registry value is found, and false otherwise.

bool bCheckCsgoInstall(char testDir[k_max_path]);
//Precondition: The array testDir is modifiable.
//Postcondition: If a CS:GO manifest file is found in testDir, it is parsed for the CS:GO installation directory.
	//If the installation directory is parsed from the manifest file, testDir is filled with it.
	//Returns true if the installation directory is parsed from the manifest file, and false otherwise.

bool bSearchSteamLibs(char testDir[k_max_path]);
//Precondition: The array testDir is modifiable.
//Postcondition: If Steam library file is found in testDir, it is parsed to enumerate up to 9 alternate library paths to search.
	//testDir is filled with one alternate library path at a time and passed to the bCheckCsgoInstall function.
	//Returns true and stops enumerating if bCheckCsgoInstall returns true for an alternate library path, and false otherwise.

bool bReadWeapFile(char csgoDir[k_max_path], string weapNames[k_num_weap], string attrData[][k_num_attr]);
//Precondition: 
//Postcondition:

bool bReadModelFiles();
//Precondition: 
//Postcondition:

bool bUnpackModels();
//Precondition: 
//Postcondition:

bool bDecompileModels();
//Precondition: 
//Postcondition:

void checkArchiveUpdate(string weapNamesAlt[][k_num_weap], string attrData[][k_num_attr], string archiveAttrData[][k_num_attr]);
//Precondition: 
//Postcondition: 

void readArchiveSir(ifstream &archiveFile, string archiveAttrData[][k_num_attr]);
//Precondition: The file input stream can be successfully connected to a file.
//Postcondition: Parses archived hitbox and weapon data from the archive file and stores it in program memory.

void readArchiveBbox();
//Precondition: 
//Postcondition: 

bool bWriteArchiveFile(string weapNamesAlt[][k_num_weap], string attrData[][k_num_attr]);
//Precondition: 
//Postcondition: 

void openArchiveFile(ifstream &archiveFile, const char k_which_archive[]);
void openArchiveFile(ofstream &archiveFile, const char k_which_archive[]);
//Precondition: 
//Postcondition:

int pickModel();
//Precondition: 
//Postcondition:

int pickWeapon();
//Precondition: 
//Postcondition:

bool bUserModifyData();
//Precondition: 
//Postcondition:

void calcIdealFreq();
//Precondition: 
//Postcondition:

int main()
{
	int menuOption = 1;
	ifstream archiveSir;
	char testDir[k_max_path];
	string weapNamesAlt[2][k_num_weap]; //First array is weapon names, second is names for alt firing mode that can be used
	string attrData[k_num_weap + 1][k_num_attr]; //Extra index to hold the attribute names in last array
	string archiveAttrData[k_num_weap][k_num_attr];

	if (getDelimitedSlice(archiveSir, k_sir, weapNamesAlt[0], k_num_weap, false, 2) != k_num_weap ||
		getDelimitedSlice(archiveSir, k_sir, weapNamesAlt[1], k_num_weap, false, 2, ',', 23) != k_num_weap ||
		getDelimitedSlice(archiveSir, k_sir, attrData[k_num_weap], k_num_attr, true, 2) != k_num_attr)
	{
		cout << "Failed to correctly retrieve weapon name or attribute list.\n\n\n";

		hitEnterToExit();
	}

	do
	{
		bool bRevertToArchive = true;

		if (menuOption == 1)
		{
			if (bGetSteamDir(testDir))
			{
				char steamappsFolder[] = "\\steamapps";

				cout << "Steam installation found in directory:\n" << testDir << endl << endl;

				concatCharArrays(testDir, steamappsFolder, testDir, k_max_path);

				if (bCheckCsgoInstall(testDir) //CSGO found in default Steam library
					|| bSearchSteamLibs(testDir)) //CSGO found in alternate Steam library
				{
					cout << "CS:GO installation found in directory:\n" << testDir << endl << endl;

					if (bReadWeapFile(testDir, weapNamesAlt[0], attrData) && bReadModelFiles())
					{
						bRevertToArchive = false;
						checkArchiveUpdate(weapNamesAlt, attrData, archiveAttrData);
					}
				}
			}

			if (bRevertToArchive)
			{
				cout << "CS:GO installation not found. Reading hitbox and weapon data from archive file.\n\n";

				readArchiveSir(archiveSir, archiveAttrData);
				readArchiveBbox();
			}

			pickModel();
			pickWeapon();
		}

		if (menuOption < 3)
			bUserModifyData();

		//******Ask user to enter a distance******
		calcIdealFreq();

	} while (bUserMenu(menuOption));

	return 0;
}

bool bUserMenu(int &menuOption)
{
	bool bContinue = true;
	
	do
	{
		cout << endl;
		cout << "1 - start over for a fresh calculation, starting from actual game data\n";
		cout << "2 - modify hitbox and weapon data for another calculation\n";
		cout << "3 - pick distance for another calculation with the same hitbox and weapon data\n";
		cout << "4 - exit the program\n\n";
		cout << "Please enter a choice from the preceding menu options: ";

		switch (menuOption = takeOnlyOneInt("1234", 4))
		{
		case 1:
		case 2:
		case 3:
			cout << endl << endl << endl; //Add some padding for new calculations
			break;

		case 4:
			cout << endl;
			bContinue = false;
			break;

		default:
			cout << endl << "That is not a valid menu option.\n\n";
		}
	} while (!static_cast<bool>(menuOption)); //Loop until a valid menu option is input

	return bContinue;
}

int takeOnlyOneInt(const char validChars[], const int numValidChars)
{
	int integer = 0;
	char character;

	if (bTakeOnlyOneChar(character))
	{
		for (int i = 0; i < numValidChars; ++i)
		{
			if (character == validChars[i])
			{
				integer = charDigitToInt(character);
				break;
			}
		}
	}

	return integer;
}

bool bTakeOnlyOneChar(char &character)
{
	bool bValidInput = false;
	bool bFirstChar = true;
	char input;

	do
	{
		cin.get(input);

		if (bFirstChar)
		{
			character = input;
			bValidInput = true;
			bFirstChar = false;
		}
		else if (input != '\n')
			bValidInput = false;

	} while (input != '\n');

	return bValidInput;
}

int charDigitToInt(const char charDigit)
{
	return (static_cast<int>(charDigit) - static_cast<int>('0'));
}

char intDigitToChar(const int intDigit)
{
	return (static_cast<char>(intDigit + static_cast<int>('0')));
}

bool bIsDigit(char character)
{
	bool bIsDigit = false;
	char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	for (int i = 0; i < 10; ++i)
	{
		if (character == digits[i])
		{
			bIsDigit = true;
			break;
		}
	}

	return bIsDigit;
}

void hitEnterToExit()
{
	char exitChar;

	cout << endl << "Hit enter to exit: ";

	bTakeOnlyOneChar(exitChar);

	exit(1);
}

int getDelimitedSlice(ifstream &delimitedFile, const char k_filename[],	string parsedSlice[], const int k_max_elements,
	                  const bool bSliceIsRow, const int k_skip_to_element, const char k_delimiter, const int k_num_slice)
{
	int numElements = 0;
	char character;
	int maxNumSlice;
	int targetNumElements;

	if (bSliceIsRow)
	{
		maxNumSlice = getNumRows(delimitedFile, k_filename, k_delimiter, k_skip_to_element);
		targetNumElements = getNumColumns(delimitedFile, k_filename, k_delimiter, k_num_slice) - k_skip_to_element + 1;
	}
	else
	{
		maxNumSlice = getNumColumns(delimitedFile, k_filename, k_delimiter, k_skip_to_element);
		targetNumElements = getNumRows(delimitedFile, k_filename, k_delimiter, k_num_slice) - k_skip_to_element + 1;
	}

	if (k_num_slice <= maxNumSlice) //Check that the slice exists
	{
		delimitedFile.open(k_filename);

		if (!delimitedFile.fail()) //Single = is intentional
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
		}

		delimitedFile.close();
	}

	return numElements;
}

int getNumColumns(ifstream &delimitedFile, const char k_filename[], const char k_delimiter, const int k_num_row)
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
	}

	delimitedFile.close();

	return numColumns;
}

int getNumRows(ifstream &delimitedFile, const char k_filename[], const char k_delimiter, const int k_num_column)
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
	}
	delimitedFile.close();

	return numRows;
}

void skipToRowNum(ifstream &delimitedFile, char &character, const int k_num_row)
{
	for (int i = 1; i < k_num_row; ++i)
	{
		do
		{
			delimitedFile.get(character);
		} while (!delimitedFile.eof() && character != '\n');
	}
}

bool bSkipToColumnNum(ifstream &delimitedFile, char &character, const char k_delimiter, const int k_num_column)
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

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], const int k_size_array)
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

int parseTextFile(string searchTerm, ifstream &searchFile, char searchResults[][k_max_path], const int k_max_results,
                  const char k_ignore_chars[], const int k_num_ignore_chars, const char k_ret_char)
{
	int instancesFound = 0;
	string testString;
	char character;

	searchFile >> testString;

	while (!searchFile.eof() && instancesFound < k_max_results)
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
					searchResults[instancesFound][i] = character;
					++i;
				}

				characterLast = character;
				searchFile.get(character);
			}

			searchResults[instancesFound][i] = '\0'; //Add terminal null character to character array
			++instancesFound;

			if (character == k_ret_char)
				break;
		}
		else
			searchFile >> testString;
	}

	return instancesFound;
}

bool bGetSteamDir(char steamDir[k_max_path])
{
	bool bFoundRegValue = false;

	//******Windows functions RegOpenKeyEx, RegQueryValueEx, and RegCloseKey search for the Steam directory if it exists******
	bFoundRegValue = true; //Set to true until Windows functions are added

	if (bFoundRegValue == true)
	{
		char steamDirTemp[] = "C:\\Program Files (x86)\\Steam"; //Replace with RegQueryValueEx output
		char blank[] = "";

		concatCharArrays(steamDirTemp, blank, steamDir, k_max_path);
	}

	return bFoundRegValue;
}

bool bCheckCsgoInstall(char testDir[k_max_path])
{
	bool bFoundCsgoInstall = false;
	ifstream manifest;

	manifest.open(static_cast<string>(testDir) + static_cast<string>("\\appmanifest_730.acf"));

	if (!manifest.fail())
	{
		char searchResult[1][k_max_path];

		//Verify CS:GO installation directory listed in manifest file contents
		if (static_cast<bool>(parseTextFile(static_cast<string>("\"installdir\""), manifest, searchResult, 1, "\t\"\0", 2)))
		{
			bFoundCsgoInstall = true;
			char installSubDir[] = "\\common\\";

			concatCharArrays(testDir, installSubDir, testDir, k_max_path);
			concatCharArrays(testDir, searchResult[0], testDir, k_max_path);
		}

		manifest.close();
	}

	return bFoundCsgoInstall;
}

bool bSearchSteamLibs(char testDir[k_max_path])
{
	bool bFoundCsgoInstall = false;
	ifstream libFile;

	libFile.open(static_cast<string>(testDir) + static_cast<string>("\\libraryfolders.vdf"));

	if (!libFile.fail())
	{
		for (int i = 1; i < 10; i++)
		{
			char searchTerm[] = { '\"', intDigitToChar(i), '\"', '\0' };
			char searchResult[1][k_max_path];

			if (static_cast<bool>(parseTextFile(static_cast<string>(searchTerm), libFile, searchResult, 1, "\t\"\0", 2)))
			{
				char steamappsFolder[] = "\\steamapps";

				concatCharArrays(searchResult[0], steamappsFolder, testDir, k_max_path);

				if (bCheckCsgoInstall(testDir))
				{
					bFoundCsgoInstall = true;
					break;
				}
			}
			else
				break;
		}

		libFile.close();
	}

	return bFoundCsgoInstall;
}

bool bReadWeapFile(char csgoDir[k_max_path], string weapNames[k_num_weap], string attrData[][k_num_attr])
{
	//Constants relating to CS:GO game data in items_game.txt
	const int k_num_unparsed_attr = 70;
	const int k_data_len = 10;
	const int k_attr_len = 30;

	bool bParseSuccess = false;
	ifstream weaponFile;

	weaponFile.open(static_cast<string>(csgoDir) + static_cast<string>("\\csgo\\scripts\\items\\items_game.txt"));

	if (!weaponFile.fail())
	{
		for (int i = 0; i < k_num_weap; ++i) //Collect weapon data for each weapon
		{
			char searchResult[1][k_max_path];
			string searchTerm = static_cast<string>("\"") + weapNames[i] + static_cast<string>("_prefab\"");
			char unparsedData[k_num_unparsed_attr][k_max_path];
			char parsedWeapData[k_num_unparsed_attr][k_data_len];
			int unparsedAttr;

			parseTextFile(searchTerm, weaponFile, searchResult, 1); //Read file up until attribtues are listed for each weapon
			searchTerm = static_cast<string>("\"attributes\"");
			unparsedAttr = parseTextFile(searchTerm, weaponFile, unparsedData, k_max_path, "\t\"\0", 2, '}');

			for (int j = 0; j < unparsedAttr; ++j) //Enumerate all returned unparsed attributes for each weapon
			{
				bool bNumberInUnparsedData = false; 

				for (int k = 0; k < k_attr_len; ++k) //Enumerate characters for each returned unparsed attribute
				{
					if (bIsDigit(unparsedData[j][k])) //Book's isdigit is throwing an exception for me
					{
						int l;

						bNumberInUnparsedData = true;

						//Copy up to k_data_len = 10 digit characters in parsedWeaponData
						for (l = 0; l < k_data_len && unparsedData[j][k + l] != '\0'; ++l)
						{
							parsedWeapData[j][l] = unparsedData[j][k + l];
							unparsedData[j][k + l] = '\0';
						}
						parsedWeapData[j][l] = '\0'; //Add terminal null character to character array

						//Change trailing characters to null characters, leaving parsed attribute names in unparsedData
						while (unparsedData[j][k + l] != '\0')
						{
							unparsedData[j][k + l] = '\0';
							++l;
						}
					}

					if (bNumberInUnparsedData)
						break; //Stop enumerating characters for unparsed attribute if number digit was found
				}

				for (int m = 0; m < k_num_attr; ++m) //unparsedData now is parsed attribute names, so compare to stored ones
				{
					if (unparsedData[j] == attrData[k_num_weap][m])
						attrData[i][m] = static_cast<string>(parsedWeapData[j]);
				}
			}
		}

		weaponFile.close(); //Close and reopen to start searching from the beginning
		weaponFile.open(static_cast<string>(csgoDir) + static_cast<string>("\\csgo\\scripts\\items\\items_game.txt"));

		if (!weaponFile.fail())
		{
			char defCycletime[1][k_max_path];

			parseTextFile(static_cast<string>("\"cycletime\""), weaponFile, defCycletime, 1, "\t\"\0", 2);

			for (int i = 0; i < k_num_weap; ++i)
			{
				if (attrData[i][0] == "") //Weapons missing cycletime get the default value
					attrData[i][0] = static_cast<string>(defCycletime[0]);
			}

			bParseSuccess = true;
		}

		weaponFile.close();
	}

	return bParseSuccess;
}

bool bReadModelFiles()
{
	bool bParseSuccess;

	if (bUnpackModels() && bDecompileModels())
	{
		//******Parse model files******
		bParseSuccess = true;
	}

	return bParseSuccess;
}

bool bUnpackModels()
{
	bool bSuccess;

	//******Call other program to do this or implement someone else's code******
	bSuccess = true; //Set to true until function body is coded

	return bSuccess;
}

bool bDecompileModels()
{
	bool bSuccess;

	//******Call other program to do this or implement someone else's code******
	bSuccess = true; //Set to true until function body is coded

	return bSuccess;
}

void checkArchiveUpdate(string weapNamesAlt[][k_num_weap], string attrData[][k_num_attr], string archiveAttrData[][k_num_attr])
{
	bool bUpdate = false;
	ifstream archiveFile;

	readArchiveSir(archiveFile, archiveAttrData);

	for (int i = 0; i < k_num_weap && !bUpdate; ++i) //&& !bUpdate will terminate the loop after first mismatch
	{
		for (int j = 0; j < k_num_attr && !bUpdate; ++j) //&& !bUpdate will terminate the loop after first mismatch
		{
			if (attrData[i][j] != archiveAttrData[i][j])
			{
				bUpdate = true;

				cout << "Data mismatch detected for " << weapNamesAlt[0][i] << " " << attrData[k_num_weap][j] << ":\n";
				cout << "Value from CS:GO's game files: " << attrData[i][j] << endl;
				cout << "Value from archive file: " << archiveAttrData[i][j] << endl << endl;
			}
		}
	}

	if (!bUpdate)
		; //******Check bbox collected data against archive******
	else
	{
		int menuOption = 0;

		do
		{
			cout << "Would you like to update the archive file with the new game data?\n";
			cout << "Please enter 1 for yes, or 2 for no: ";

			switch (menuOption = takeOnlyOneInt("12", 2))
			{
			case 1:
				bWriteArchiveFile(weapNamesAlt, attrData);
				break;

			case 2:
				break;

			default:
				cout << endl << "That is not a valid menu option.\n\n";
			}
		} while (!static_cast<bool>(menuOption));
	}
}

void readArchiveSir(ifstream &archiveFile, string archiveAttrData[][k_num_attr])
{
	for (int i = 0; i < k_num_weap; ++i)
		getDelimitedSlice(archiveFile, k_sir, archiveAttrData[i], k_num_attr, true, 2, ',', i + 2);
}

void readArchiveBbox()
{
	//******Parse bbox data from archive******
}

bool bWriteArchiveFile(string weapNamesAlt[][k_num_weap], string attrData[][k_num_attr])
{
	bool bWriteSuccess = false;
	ofstream archiveFile;

	openArchiveFile(archiveFile, k_sir);

	for (int j = 0; j < k_num_attr; ++j)
		archiveFile << ',' << attrData[k_num_weap][j];

	archiveFile << ",use alt mode";
	archiveFile << endl;

	for (int i = 0; i < k_num_weap; ++i)
	{
		archiveFile << weapNamesAlt[0][i];

		for (int j = 0; j < k_num_attr; ++j)
			archiveFile << ',' << attrData[i][j];

		archiveFile << ',' << weapNamesAlt[1][i];
		archiveFile << endl;
	}

	archiveFile.close();
	//openArchiveFile(archiveFile, k_bbox);
	//******Write bbox data******
	//archiveFile.close();

	cout << endl << "Archive file updated." << endl << endl;

	bWriteSuccess = true;

	return bWriteSuccess;
}

void openArchiveFile(ifstream &archiveFile, const char k_which_archive[])
{
	archiveFile.open(k_which_archive);

	if (archiveFile.fail())
	{
		cout << endl << "Archive file " << k_which_archive << " failed to open.\n\n\n";

		hitEnterToExit();
	}
}

void openArchiveFile(ofstream &archiveFile, const char k_which_archive[])
{
	archiveFile.open(k_which_archive);

	if (archiveFile.fail())
	{
		cout << endl << "Archive file " << k_which_archive << " failed to open.\n\n\n";

		hitEnterToExit();
	}
}

int pickModel()
{
	int modelIndex;

	//******Display a list of models and ask user to pick one******
	modelIndex = 0; //Set to 0 until function body is coded

	return modelIndex;
}

int pickWeapon()
{
	int weaponIndex;

	//******Display a list of weapons and ask user to pick one******
	weaponIndex = 0; //Set to 0 until function body is coded

	return weaponIndex;
}

bool bUserModifyData()
{
	bool bDataModified = false;

	//******Dialog to figure out which value(s) to modify******

	return bDataModified;
}

void calcIdealFreq()
{
	//******Amazing maths go here******
}
