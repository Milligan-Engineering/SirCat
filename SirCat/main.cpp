//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Program to calculate the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 9, 2018

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
//#include <Windows.h> needed when Windows functions are used
//Consider converting project to unicode and wide characters to support languages using non-ASCII characters

using namespace std;

bool getSteamDir(char steamDir[_MAX_PATH]);
//Precondition: The array steamDir is modifiable.
//Postcondition: If a valid registry value for a Steam installation is found, steamDir is filled with the installation directory.
//               Returns true if a valid registry value is found, and false otherwise.

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray);
//Precondition: The arrays cArray1 and cArray2 are null-terminated.
//              The sum of filled elements of cArray1 and cArray2 is less than or equal to the size of array concatArray.
//              concatArray is modifiable.
//              sizeConcatArray is the size of concatArray.
//Postcondition: concatArray is filled with the concatenation of cArray1 and cArray2, and terminated with a null character.
//               cArray1 and cArray2 can be the same array as concatArray to append or prepend to the original input array.
//               Returns the index of the last filled element in concatArray.

int parseTextFile(string searchTerm, ifstream &searchFile, char searchResults[][_MAX_PATH], int maxSearchResults,
                  const char ignoreChars[] = "", int numIgnoreChars = 0);
//Precondition: searchTerm should not contain any whitespace characters.
//              The file input stream searchFile has been successfully connected to a file with ifstream::open member function.
//              The two-dimensional array searchResult is modifiable.
//              maxSearchResults is less than or equal to the first dimension in searchResult.
//              If the array ignoreChars is passed, numIgnoreChars is less than or equal to the size of ignoreChars.
//Postcondition: searchFile is searched by looping through non-whitespace strings that are separated by whitespace characters.
//               When a string matches searchTerm, a *searchResults[_MAX_PATH] array is filled with the line's remaining characters.
//               Characters passed in ignoreChars are not copied to *searchResults[_MAX_PATH] and escaped backslashes are unescaped.
//               searchResult is filled, up to maxSearchResults number of *searchResults[_MAX_PATH] arrays.
//               Returns the number of *searchResults[_MAX_PATH] arrays that are at least partially filled.

bool checkCsgoInstall(char testDir[_MAX_PATH]);
//Precondition: The array testDir is modifiable.
//Postcondition: If a CS:GO manifest file is found in testDir, it is parsed for the CS:GO installation directory.
//               If the installation directory is parsed from the manifest file, testDir is filled with it.
//               Returns true if the installation directory is parsed from the manifest file, and false otherwise.

bool searchSteamLibraries(char testDir[_MAX_PATH]);
//Precondition: The array testDir is modifiable.
//Postcondition: If Steam library folders file is found in testDir, it is parsed to enumerate up to 9 alternate library paths to search.
//               testDir is filled with one alternate library path at a time and passed to the checkCsgoInstall function.
//               Returns true and stops enumerating if checkCsgoInstall returns true for an alternate library path, and false otherwise.

void readArchiveFile();
//Precondition: The archive file exists and can be successfully connected to a file input stream with ifstream::open member function.
//Postcondition: Parses archived hitbox and weapon data from the archive file and stores it in program memory.

int takeOnlyOneChar();
//Precondition: The returned value is intended to be a decimal digit.
//Postcondition: Returns int equal to first input character if it is a decimal digit, and 0 otherwise, discarding the rest of the input.

int main()
{
	int menuOption = 0;
	char testDir[_MAX_PATH];
	bool bExit = false;

	do
	{
		bool bRevertToArchiveFile = true;

		if (menuOption < 2)
		{
			if (getSteamDir(testDir))
			{
				char steamappsFolder[] = "\\steamapps";

				cout << "Steam installation found in directory:\n" << testDir << endl << endl;

				concatCharArrays(testDir, steamappsFolder, testDir, _MAX_PATH);
				if (checkCsgoInstall(testDir) //CSGO found in default Steam library
					|| searchSteamLibraries(testDir)) //CSGO found in alternate Steam library
				{
					cout << "CS:GO installation found in directory:\n" << testDir << endl << endl;

					//readWeaponFile
					//unpackModels
					//decompileModels
					//readModelFiles
					//overwriteArchiveFile
					bRevertToArchiveFile = false;
				}
			}

			if (bRevertToArchiveFile)
			{
				cout << "CS:GO installation not found. Reading hitbox and weapon data from archive file.\n\n";

				readArchiveFile();
			}

			//hitboxList
			//weaponList
		}

		if (menuOption < 3)
		{
			//userModifyData
		}

		//enter distance

		//calcIdealFreq

		bool bValidMenuOption;

		do
		{
			bValidMenuOption = false;

			cout << endl << endl;
			cout << "1 - start over for a fresh calculation, starting from actual game data\n";
			cout << "2 - modify hitbox and weapon data for another calculation\n";
			cout << "3 - pick distance for another calculation with the same hitbox and weapon data\n";
			cout << "4 - exit the program\n\n";
			cout << "Please enter a choice from the preceding menu options: ";

			switch (menuOption = takeOnlyOneChar())
			{
			case 1:
			case 2:
			case 3:
				cout << endl << endl << endl << endl << endl;
				bValidMenuOption = true;
				break;
			case 4:
				bExit = true;
				bValidMenuOption = true;
				break;
			default:
				cout << endl << "That is not a valid menu option.\n\n";
			}
		} while (!bValidMenuOption);
	} while (!bExit);

	return 0;
}

bool getSteamDir(char steamDir[_MAX_PATH])
{
	bool bFoundRegValue = false;

	//Use Windows functions RegOpenKeyEx, RegQueryValueEx, and RegCloseKey to search for the Steam directory if it exists
	bFoundRegValue = true; //Set to true until Windows functions are added

	if (bFoundRegValue == true)
	{
		char steamDirTemp[] = "C:\\Program Files (x86)\\Steam"; //Replace with RegQueryValueEx output
		char blank[] = "";

		concatCharArrays(steamDirTemp, blank, steamDir, _MAX_PATH);
	}

	return bFoundRegValue;
}

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray)
{
	int concatIndex = 0;

	for (int i = 0; cArray1[i] != '\0' && concatIndex < sizeConcatArray - 1; ++i)
	{
		concatArray[concatIndex] = cArray1[i];
		++concatIndex;
	}

	for (int i = 0; cArray2[i] != '\0' && concatIndex < sizeConcatArray - 1; ++i)
	{
		concatArray[concatIndex] = cArray2[i];
		++concatIndex;
	}

	concatArray[concatIndex] = '\0';

	return concatIndex;
}

int parseTextFile(string searchTerm, ifstream &searchFile, char searchResults[][_MAX_PATH], int maxSearchResults,
                  const char ignoreChars[], int numIgnoreChars)
{
	int instancesFound = 0;
	string testString;

	searchFile >> testString;
	while (!searchFile.eof() && instancesFound < maxSearchResults)
	{
		if (testString == searchTerm)
		{
			char character;
			char characterLast = '\0';
			int i = 0;

			searchFile.get(character);
			while(!searchFile.eof() && character != '\n') //Fill search result entry until new-line or end of file
			{
				bool bIgnoreChar = false;

				for (int j = 0; j < numIgnoreChars; ++j)
				{
					if (character == ignoreChars[j]
						|| (characterLast == '\\' && character == '\\')) //Detect escaped backslashes
						bIgnoreChar = true;
				}

				if (!bIgnoreChar)
				{
					searchResults[instancesFound][i] = character;
					++i;
				}

				characterLast = character;
				searchFile.get(character);
			}
			searchResults[instancesFound][i] = '\0';

			++instancesFound;
		}
		else
			searchFile >> testString;
	}

	return instancesFound;
}

bool checkCsgoInstall(char testDir[_MAX_PATH])
{
	char manifestFileName[] = "\\appmanifest_730.acf";
	char manifestFilePath[_MAX_PATH];
	ifstream manifestFile;
	bool bFoundCsgoInstall = false;

	concatCharArrays(testDir, manifestFileName, manifestFilePath, _MAX_PATH);
	manifestFile.open(manifestFilePath);
	if (!manifestFile.fail())
	{
		char searchResult[1][_MAX_PATH];
		const char ignoreChars[] = { '\t', '\"', '\0' }; //Ignore tabs and quotations for searchResult

		if (static_cast<bool>(parseTextFile(static_cast<string>("\"installdir\""), manifestFile, searchResult, 1, ignoreChars, 2)))
		{
			bFoundCsgoInstall = true;
			char installSubDir[] = "\\common\\";

			concatCharArrays(testDir, installSubDir, testDir, _MAX_PATH);
			concatCharArrays(testDir, searchResult[0], testDir, _MAX_PATH);
		}

		manifestFile.close();
	}

	return bFoundCsgoInstall;
}

bool searchSteamLibraries(char testDir[_MAX_PATH])
{
	char libFoldersFileName[] = "\\libraryfolders.vdf";
	char libFoldersFilePath[_MAX_PATH];
	ifstream libFoldersFile;
	bool bFoundCsgoInstall = false;

	concatCharArrays(testDir, libFoldersFileName, libFoldersFilePath, _MAX_PATH);
	libFoldersFile.open(libFoldersFilePath);
	if (!libFoldersFile.fail())
	{
		for (int i = 1; i < 10; i++)
		{
			char searchTerm[] = { '\"', static_cast<char>(i + static_cast<int>('0')), '\"', '\0' };
			char searchResult[1][_MAX_PATH];
			const char ignoreChars[] = { '\t', '\"', '\0' }; //Ignore tabs and quotations for searchResult

			if (static_cast<bool>(parseTextFile(static_cast<string>(searchTerm), libFoldersFile, searchResult, 1, ignoreChars, 2)))
			{
				char steamappsFolder[] = "\\steamapps";

				concatCharArrays(searchResult[0], steamappsFolder, testDir, _MAX_PATH);
				if (checkCsgoInstall(testDir))
				{
					bFoundCsgoInstall = true;
					break;
				}
			}
			else
				break;
		}

		libFoldersFile.close();
	}

	return bFoundCsgoInstall;
}

void readArchiveFile()
{
	ifstream archiveFile;
	
	archiveFile.open("archive.csv");
	if (archiveFile.fail())
	{
		char exitLetter;

		cout << endl << "Archive file failed to open.\n\n\n";
		cout << endl << "Type a letter to exit: ";
		cin >> exitLetter;
		exit(1);
	}

	char character;

	cout << endl;
	archiveFile.get(character);
	while (!archiveFile.eof())
	{
		//Parse archive file contents and store to program memory
		//Waiting to decide on file format and handling of memory

		archiveFile.get(character);
	}
	archiveFile.close();
}

int takeOnlyOneChar()
{
	int menuOption = 0;
	char menuOptionChar = '\0';

	for (bool bFirstChar = true; menuOptionChar != '\n';)
	{
		cin.get(menuOptionChar);

		if (bFirstChar)
		{
			menuOption = static_cast<int>(menuOptionChar) - static_cast<int>('0');
			if (menuOption < 0 || menuOption > 9)
				menuOption = 0;

			bFirstChar = false;
		}
		else if (menuOptionChar != '\n')
			menuOption = 0;
	}

	return menuOption;
}