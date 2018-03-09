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

void pauseBeforeExit();
//Postcondition: Console will wait for a user-inputted character before exiting.

bool getSteamDir(char steamDir[]);
//Precondition: 
//Postcondition: 

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray);
//Precondition: 
//Postcondition: 

int parseTextFile(string searchTerm, ifstream &searchFile, char searchResults[][_MAX_PATH], int maxSearchResults,
                  char ignoreChars[], int numIgnoreChars);
//Precondition: 
//Postcondition: 

bool checkCsgoInstall(char testDir[]);
//Precondition: 
//Postcondition: 

bool searchSteamLibraries(char testDir[]);
//Precondition: 
//Postcondition: 

void readArchiveFile();
//Postcondition: 

int main()
{
	char steamDir[_MAX_PATH];
	char testDir[_MAX_PATH];
	bool bRevertToArchiveFile = true;

	if (getSteamDir(steamDir))
	{
		char defaultFolder[] = "\\steamapps";

		cout << "Steam installation found in directory: \n" << steamDir << endl << endl;

		concatCharArrays(steamDir, defaultFolder, testDir, _MAX_PATH);
		if (checkCsgoInstall(testDir)) //CSGO found in default Steam library
			bRevertToArchiveFile = false;
		else if (searchSteamLibraries(testDir)) //CSGO found in alternate Steam library
			bRevertToArchiveFile = false;
	}

	if (bRevertToArchiveFile)
	{
		cout << "CS:GO installation not found. Reading hitbox and weapon data from archive file.\n\n";

		readArchiveFile();
	}
	else
	{
		cout << "CS:GO installation found in directory: \n" << testDir << endl << endl;

		//readWeaponFile();
	}

	pauseBeforeExit();

	return 0;
}

void pauseBeforeExit()
{
	char exitLetter;

	cout << "\n\nType a letter to exit: ";
	cin >> exitLetter;
}

bool getSteamDir(char steamDir[])
{
	bool bFoundRegValue = false;

	//Use Windows functions RegOpenKeyEx, RegQueryValueEx, and RegCloseKey to search for the Steam directory if it exists
	bFoundRegValue = true; //Set to true until Windows functions are added

	if (bFoundRegValue == true)
	{
		char steamDirTemp[] = "C:\\Program Files (x86)\\Steam"; //Replace with RegQueryValueEx output

		for (int i = 0; i < 29; ++i) //Size of steamDirTemp is 29
			steamDir[i] = steamDirTemp[i];
	}

	return bFoundRegValue;
}

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray)
{
	int concatIndex = 0;

	for (int i = 0; cArray1[i] != '\0' && concatIndex < sizeConcatArray; ++i)
	{
		concatArray[concatIndex] = cArray1[i];
		++concatIndex;
	}

	for (int i = 0; cArray2[i] != '\0' && concatIndex < sizeConcatArray; ++i)
	{
		concatArray[concatIndex] = cArray2[i];
		++concatIndex;
	}

	if (concatIndex < sizeConcatArray)
	{
		concatArray[concatIndex] = '\0';
		++concatIndex;
	}

	return concatIndex;
}

char g_blankArray[] = "";

int parseTextFile(string searchTerm, ifstream &searchFile, char searchResults[][_MAX_PATH], int maxSearchResults,
                  char ignoreChars[] = g_blankArray, int numIgnoreChars = 0)
{
	int instancesFound = 0;
	string testString;

	searchFile >> testString;
	while (!searchFile.eof()
		&& instancesFound < maxSearchResults)
	{
		if (testString == searchTerm)
		{
			char character;
			int i = 0;

			searchFile.get(character);
			while(!searchFile.eof() && character != '\n') //Fill search result entry until new-line or end of file
			{
				bool bIgnoreChar = false;

				for (int j = 0; j < numIgnoreChars; ++j)
				{
					if (character == ignoreChars[j])
						bIgnoreChar = true;
				}

				if (!bIgnoreChar)
				{
					searchResults[instancesFound][i] = character;
					++i;
				}

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

bool checkCsgoInstall(char testDir[])
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
		char ignoreChars[] = { '\t', '\"' }; //Ignore tabs and quotations for searchResult

		if (static_cast<bool>(parseTextFile("\"installdir\"", manifestFile, searchResult, 1, ignoreChars, 2)))
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

bool searchSteamLibraries(char testDir[])
{
	bool bFoundCsgoInstall = false;

	//Enumerate alternate Steam libraries from defaultDir\libraryfolders.vdf

	//Loop calls to checkCsgoInstall for each alternate Steam library
	//If CS:GO installation is found, set bFoundCsgoInstall to true and exit loop

	return bFoundCsgoInstall;
}

void readArchiveFile()
{
	ifstream archiveFile;
	
	archiveFile.open("archive.csv");
	if (archiveFile.fail())
	{
		cout << endl << "Archive file failed to open.\n\n";
		pauseBeforeExit();
		exit(1);
	}

	char character;

	cout << endl;
	archiveFile.get(character);
	while (!archiveFile.eof())
	{
		//Parse archive file contents

		archiveFile.get(character);
	}
	archiveFile.close();
}