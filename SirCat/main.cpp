//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 10, 2018

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
//#include <Windows.h> needed when Windows functions are used
//Consider converting project to unicode and wide characters to support languages using non-ASCII characters

using namespace std;

bool bUserMenu(int &menuOption);
//Postcondition: menuOption from the calling function is updated according to the user's input.
	//Returns true if the program should continue or false if the program should exit.

bool bGetSteamDir(char steamDir[_MAX_PATH]);
//Precondition: The array steamDir is modifiable.
//Postcondition: If a valid registry value for a Steam installation is found, steamDir is filled with the installation directory.
	//Returns true if a valid registry value is found, and false otherwise.

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray);
//Precondition: The strings cArray1 and cArray2 are null-terminated.
	//The sum of filled elements of cArray1 and cArray2 is less than or equal to the size of array concatArray.
	//concatArray is modifiable.
	//sizeConcatArray is the size of concatArray.
//Postcondition: concatArray is filled with the concatenation of cArray1 and cArray2, and terminated with a null character.
	//cArray1 or cArray2 can be the same array as concatArray to append or prepend to the original string.
	//Returns the index of the last filled element in concatArray.

int parseTextFile(string searchTerm, ifstream &searchFile, char searchRes[][_MAX_PATH], int maxSearchResults,
                  const char ignoreChar[] = "", int numIgnoreChar = 0);
//Precondition: searchTerm should not contain any whitespace characters.
	//The file input stream searchFile has been successfully connected to a file with ifstream::open member function.
	//The two-dimensional array searchRes is modifiable.
	//maxSearchResults is less than or equal to the first dimension in searchRes.
	//If the array ignoreChar is passed, numIgnoreChar is less than or equal to the size of ignoreChar.
//Postcondition: searchFile is searched by looping through non-whitespace strings that are separated by whitespace characters.
	//When a string matches searchTerm, a *searchRes[_MAX_PATH] array is filled with the line's remaining characters.
	//Characters passed in ignoreChar are not copied to *searchRes[_MAX_PATH] and escaped backslashes are unescaped.
	//searchRes is filled, up to maxSearchResults number of null-terminated strings *searchRes[_MAX_PATH].
	//Returns the number of null-terminated strings *searchRes[_MAX_PATH] filled in the array of strings searchRes.

bool bCheckCsgoInstall(char testDir[_MAX_PATH]);
//Precondition: The array testDir is modifiable.
//Postcondition: If a CS:GO manifest file is found in testDir, it is parsed for the CS:GO installation directory.
	//If the installation directory is parsed from the manifest file, testDir is filled with it.
	//Returns true if the installation directory is parsed from the manifest file, and false otherwise.

bool bSearchSteamLibs(char testDir[_MAX_PATH]);
//Precondition: The array testDir is modifiable.
//Postcondition: If Steam library file is found in testDir, it is parsed to enumerate up to 9 alternate library paths to search.
	//testDir is filled with one alternate library path at a time and passed to the bCheckCsgoInstall function.
	//Returns true and stops enumerating if bCheckCsgoInstall returns true for an alternate library path, and false otherwise.

void readArchiveFile();
//Precondition: The archive file can be successfully connected to a file input stream with ifstream::open member function.
//Postcondition: Parses archived hitbox and weapon data from the archive file and stores it in program memory.

int takeOnlyOneChar();
//Precondition: The returned value is intended to be a decimal digit.
//Postcondition: Returns int equal to input char decimal digit if it is immediately followed by input of new-line, and 0 otherwise.

int main()
{
	int menuOption = 0;
	char testDir[_MAX_PATH];

	do
	{
		bool bRevertToArchiveFile = true;

		if (menuOption < 2)
		{
			if (bGetSteamDir(testDir))
			{
				char steamappsFolder[] = "\\steamapps";

				cout << "Steam installation found in directory:\n" << testDir << endl << endl;

				concatCharArrays(testDir, steamappsFolder, testDir, _MAX_PATH);
				if (bCheckCsgoInstall(testDir) //CSGO found in default Steam library
					|| bSearchSteamLibs(testDir)) //CSGO found in alternate Steam library
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

	} while (bUserMenu(menuOption));

	return 0;
}

bool bUserMenu(int &menuOption)
{
	bool bInvalidMenuOption;
	bool bContinue = true;

	do
	{
		bInvalidMenuOption = true;

		cout << endl << endl;
		cout << "1 - start over for a fresh calculation, starting from actual game data\n";
		cout << "2 - modify hitbox and weapon data for another calculation\n";
		cout << "3 - pick distance for another calculation with the same hitbox and weapon data\n";
		cout << "4 - exit the program\n\n";
		cout << "Please enter a choice from the preceding menu options: ";

		switch (menuOption = takeOnlyOneChar()) //Change menuOption in calling function
		{
		case 1:
		case 2:
		case 3:
			cout << endl << endl << endl; //Add some padding for new calculations
			bInvalidMenuOption = false;
			break;

		case 4:
			cout << endl; //End program with a new-line instruction
			bContinue = false;
			bInvalidMenuOption = false;
			break;

		default:
			cout << endl << "That is not a valid menu option.\n\n";
		}

	} while (bInvalidMenuOption);

	return bContinue;
}

bool bGetSteamDir(char steamDir[_MAX_PATH])
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

int parseTextFile(string searchTerm, ifstream &searchFile, char searchRes[][_MAX_PATH], int maxSearchResults,
                  const char ignoreChar[], int numIgnoreChar)
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

				for (int j = 0; j < numIgnoreChar; ++j)
				{
					if (character == ignoreChar[j]
						|| (characterLast == '\\' && character == '\\')) //Detect escaped backslashes
						bIgnoreChar = true;
				}

				if (!bIgnoreChar)
				{
					searchRes[instancesFound][i] = character;
					++i;
				}

				characterLast = character;
				searchFile.get(character);
			}
			searchRes[instancesFound][i] = '\0'; //Terminate string with null character

			++instancesFound;
		}
		else
			searchFile >> testString;
	}

	return instancesFound;
}

bool bCheckCsgoInstall(char testDir[_MAX_PATH])
{
	char manifestName[] = "\\appmanifest_730.acf";
	char manifestPath[_MAX_PATH];
	ifstream manifest;
	bool bFoundCsgoInstall = false;

	concatCharArrays(testDir, manifestName, manifestPath, _MAX_PATH);
	manifest.open(manifestPath);
	if (!manifest.fail())
	{
		char searchRes[1][_MAX_PATH];
		const char ignoreChar[] = { '\t', '\"', '\0' }; //Ignore tabs and quotations for searchRes

		if (static_cast<bool>(parseTextFile(static_cast<string>("\"installdir\""), manifest, searchRes, 1, ignoreChar, 2)))
		{
			bFoundCsgoInstall = true;
			char installSubDir[] = "\\common\\";

			concatCharArrays(testDir, installSubDir, testDir, _MAX_PATH);
			concatCharArrays(testDir, searchRes[0], testDir, _MAX_PATH);
		}

		manifest.close();
	}

	return bFoundCsgoInstall;
}

bool bSearchSteamLibs(char testDir[_MAX_PATH])
{
	char libName[] = "\\libraryfolders.vdf";
	char libPath[_MAX_PATH];
	ifstream lib;
	bool bFoundCsgoInstall = false;

	concatCharArrays(testDir, libName, libPath, _MAX_PATH);
	lib.open(libPath);

	if (!lib.fail())
	{
		for (int i = 1; i < 10; i++)
		{
			char searchTerm[] = { '\"', static_cast<char>(i + static_cast<int>('0')), '\"', '\0' };
			char searchRes[1][_MAX_PATH];
			const char ignoreChar[] = { '\t', '\"', '\0' }; //Ignore tabs and quotations for searchRes

			if (static_cast<bool>(parseTextFile(static_cast<string>(searchTerm), lib, searchRes, 1, ignoreChar, 2)))
			{
				char steamappsFolder[] = "\\steamapps";

				concatCharArrays(searchRes[0], steamappsFolder, testDir, _MAX_PATH);
				if (bCheckCsgoInstall(testDir))
				{
					bFoundCsgoInstall = true;
					break;
				}
			}
			else
				break;
		}

		lib.close();
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
	const int INVALID = 0;
	int menuOption = INVALID;
	char menuOptionChar = '\0';

	for (bool bFirstChar = true; menuOptionChar != '\n'; ) //loop-expression intentionally left blank
	{
		cin.get(menuOptionChar);

		if (bFirstChar)
		{
			//Preserve decimal digit while converting char to int
			menuOption = static_cast<int>(menuOptionChar) - static_cast<int>('0');
			if (menuOption < 0 || menuOption > 9)
				menuOption = INVALID;

			bFirstChar = false;
		}
		else if (menuOptionChar != '\n') //More than one char input
			menuOption = INVALID;
	}

	return menuOption;
}