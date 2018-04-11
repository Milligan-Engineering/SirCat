//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #09
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: April 11, 2018

#ifndef STRICT
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "csgo\BboxData.h"
#include "csgo\SirData.h"
#include "csgo\FindCsgo.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

struct Data
{
	BboxData bbox;
	SirData sir;
};

void updatePrompt(Data &archiveData, Data &newData);
//Precondition: 
//Postcondition: 

bool bUserMenu(int &menuOption);
//Precondition: menuOption is modifiable.
//Postcondition: menuOption is updated according to user input in response to a program menu.
	//Returns true if the program should continue or false if the program should exit.

int takeOnlyOneInt(const WCHAR validChars[], const int numValidChars);
//Precondition: numValidChars is the size of validChars.
//Postcondition: Get one and only one input character, convert it to int, and return the integer;

bool bTakeOnlyOneWchar(WCHAR &character);
//Precondition: character is modifiable.
//Postcondition: Sets character to first character input 

int WcharDigitToInt(const WCHAR wcharDigit);
//Precondition: 
//Postcondition: 

void hitEnterToExit();
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
	Data archiveData;
	int menuOption = 1;

	if (!archiveData.bbox.bMakeBboxObjArchive(static_cast<wstring>(L"archiveBboxData.csv"))
		|| !archiveData.sir.bMakeSirObjArchive(static_cast<wstring>(L"archiveSirData.csv")))
	{
		wcout << L"Failed to correctly retrieve archived data.\n\n\n";
		hitEnterToExit();
	}

	do
	{
		bool bRevertToArchive = true;

		if (menuOption == 1)
		{
			FindCsgo findCsgo;
			wstring steamDir;

			if (findCsgo.bFetchSteamDir(steamDir))
			{
				wcout << L"Steam installation found in directory:\n" << steamDir << endl << endl;

				if (findCsgo.bCheckCsgoInstall() //CSGO found in default Steam library
					|| findCsgo.bSearchSteamLibs()) //CSGO found in alternate Steam library
				{
					Data newData;

					wcout << L"CS:GO installation found in directory:\n" << findCsgo.getTestDir() << endl << endl;
					wcout << L"Checking fresh CS:GO hitbox and weapon data against file archive data ...";

					if (newData.bbox.bReadModelFiles(findCsgo.getTestDir())	&& newData.sir.bReadWeapFile(findCsgo.getTestDir()))
					{
						wstring badRowName, badColName, badNewVal, badArchiveVal;
						bool bUpdate = true;

						bRevertToArchive = false;
						archiveData.bbox.readArchive();
						archiveData.sir.readArchive();

						if (archiveData.bbox.bCheckArchive(newData.bbox, badRowName, badColName, badNewVal, badArchiveVal))
							wcout << endl << endl << L"Data mismatch detected in " << archiveData.bbox.getCsvName();
						else if (archiveData.sir.bCheckArchive(newData.sir, badRowName, badColName, badNewVal, badArchiveVal))
							wcout << endl << endl << L"Data mismatch detected in " << archiveData.sir.getCsvName();
						else
						{
							wcout << L"done." << endl << "No discrepancies detected.\n\n";
							bUpdate = false;
						}

						if (bUpdate)
						{
							wcout << L" for " << badRowName << L" " << badColName << L" ...\n";
							wcout << L"Value from CS:GO's game files: " << badNewVal << endl;
							wcout << L"Value from archive file: " << badArchiveVal << endl << endl;
							updatePrompt(archiveData, newData);
						}
					}
				}
			}

			if (bRevertToArchive)
			{
				wcout << endl << L"CS:GO not found or error reading its files.\n";
				wcout << L"Reading hitbox and weapon data from archive file.\n\n";
				archiveData.bbox.readArchive();
				archiveData.sir.readArchive();
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

void updatePrompt(Data &archiveData, Data &newData)
{
	int menuOption = 0;

	do
	{
		wcout << L"Would you like to update the archive file with the new game data?\n";
		wcout << L"Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt(L"12", 2))
		{
		case 1:
			if (archiveData.bbox.bWriteArchiveFile(newData.bbox) && archiveData.sir.bWriteArchiveFile(newData.sir))
			{
				archiveData.bbox.readArchive();
				archiveData.sir.readArchive();
				wcout << endl << endl << L"Archive files updated." << endl << endl;
			}
			else
				wcout << endl << endl << L"Failed to update archive files." << endl << endl;

			break;
		case 2:
			wcout << endl << endl;
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.\n\n";
		}
	} while (menuOption == 0);
}

bool bUserMenu(int &menuOption)
{
	bool bContinue = true;
	
	do
	{
		wcout << L"1 - start over for a fresh calculation, starting from actual game data\n";
		wcout << L"2 - modify hitbox and weapon data for another calculation\n";
		wcout << L"3 - pick distance for another calculation with the same hitbox and weapon data\n";
		wcout << L"4 - exit the program\n";
		wcout << L"Please enter a choice from the preceding menu options: ";

		switch (menuOption = takeOnlyOneInt(L"1234", 4))
		{
		case 1:
		case 2:
		case 3:
			wcout << endl << endl;
			break;
		case 4:
			wcout << endl;
			bContinue = false;
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.\n";
		}
	} while (menuOption == 0); //Loop until a valid menu option is input

	return bContinue;
}

int takeOnlyOneInt(const WCHAR validChars[], const int numValidChars)
{
	int integer = 0;
	WCHAR character;

	if (bTakeOnlyOneWchar(character))
	{
		for (int i = 0; i < numValidChars; ++i)
		{
			if (character == validChars[i])
			{
				integer = WcharDigitToInt(character);
				break;
			}
		}
	}

	return integer;
}

bool bTakeOnlyOneWchar(WCHAR &character)
{
	bool bValidInput = false;
	bool bFirstChar = true;
	WCHAR input;

	do
	{
		wcin.get(input);

		if (bFirstChar)
		{
			character = input;
			bValidInput = true;
			bFirstChar = false;
		}
		else if (input != L'\n')
			bValidInput = false;

	} while (input != L'\n');

	return bValidInput;
}

int WcharDigitToInt(const WCHAR wcharDigit)
{
	return (static_cast<int>(wcharDigit) - static_cast<int>(L'0'));
}

void hitEnterToExit()
{
	WCHAR exitWchar;

	wcout << endl << L"Hit enter to exit: ";
	bTakeOnlyOneWchar(exitWchar);
	wcout << endl;
	exit(1);
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
