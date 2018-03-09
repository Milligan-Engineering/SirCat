//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Program to calculate the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 8, 2018

#include <iostream>
#include <cstdlib>
#include <Windows.h> //needed when Windows functions are used
//Consider converting project to unicode and wide characters to support languages using non-ASCII characters

bool getSteamDir(char steamDir[]);
//Precondition: 
//Postcondition: 

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray);
//Precondition: 
//Postcondition: 

bool checkCsgoInstall(char testDir[]);
//Precondition: 
//Postcondition:

bool searchSteamLibraries(char steamDir[], char testDir[]);
//Precondition: 
//Postcondition:

int main()
{
	using namespace std;

	char steamDir[_MAX_PATH];
	bool bRevertToArchiveFile = true;

	if (getSteamDir(steamDir))
	{
		char defaultFolder[] = "\\steamapps";
		char testDir[_MAX_PATH];
		bool bFoundCsgoDir = false;

		cout << "Steam installation found in directory: " << steamDir << endl;

		concatCharArrays(steamDir, defaultFolder, testDir, sizeof(testDir));
		bFoundCsgoDir = checkCsgoInstall(testDir);

		if (!bFoundCsgoDir)
		{
			if (searchSteamLibraries(steamDir, testDir))
			{
				bFoundCsgoDir = true;
			}
		}

		if (bFoundCsgoDir)
		{
			cout << "CS:GO installation found in directory: " << testDir << endl;

			bRevertToArchiveFile = false;
			//readWeaponFile();
		}
	}

	if (bRevertToArchiveFile)
	{
		//readArchiveFile();
	}

	char exitLetter;

	cout << endl << "Type a letter to exit: ";
	cin >> exitLetter;

	return 0;
}

bool getSteamDir(char steamDir[])
{
	bool bFoundRegValue = false;

	//Use Windows functions RegOpenKeyEx, RegQueryValueEx, and RegCloseKey to search for the Steam directory if it exists
	bFoundRegValue = true; //Set to true until Windows functions are added

	if (bFoundRegValue == true)
	{
		char steamDirTemp[] = "C:\\Program Files (x86)\\Steam"; //Replace with RegQueryValueEx output

		for (int i = 0; i < sizeof(steamDirTemp); i++)
		{
			steamDir[i] = steamDirTemp[i];
		}

		return true;
	}

	return false;
}

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray)
{
	int concatIndex = 0;

	for (int i = 0; cArray1[i] != '\0' && concatIndex < sizeConcatArray; i++)
	{
		concatArray[concatIndex] = cArray1[i];
		++concatIndex;
	}

	for (int i = 0; cArray2[i] != '\0' && concatIndex < sizeConcatArray; i++)
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

bool checkCsgoInstall(char testDir[])
{
	bool bFoundCsgoInstall = false;

	//Use Windows functions FindFirstFile, FindNextFile, and FileClose to search for appmanifest_730.acf in the given directory
	bFoundCsgoInstall = true; //Set to true until Windows functions are added

	return bFoundCsgoInstall;
}

bool searchSteamLibraries(char steamDir[], char testDir[])
{
	char steamUserFolder[] = "\\userdata";
	char steamUserDir[_MAX_PATH];
	char steamUserSearchWildcard[] = "\\*";
	char steamUserSearchTerm[_MAX_PATH];
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	concatCharArrays(steamDir, steamUserFolder, steamUserDir, sizeof(steamUserDir));
	concatCharArrays(steamUserDir, steamUserSearchWildcard, steamUserSearchTerm, sizeof(steamUserSearchTerm));

	hFind = FindFirstFile(steamUserSearchTerm, &findFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (static_cast<bool>(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //Check if directory attribute bit is set
				&& (findFileData.cFileName[0] != '.')) //Exclude parent directories
			{
				concatCharArrays(steamUserDir, findFileData.cFileName, testDir, sizeof(testDir));

				if (checkCsgoInstall(testDir))
				{
					return true;
				}
			}
		} while (FindNextFile(hFind, &findFileData) != false);
	}
	FindClose(hFind);

	return false;
}