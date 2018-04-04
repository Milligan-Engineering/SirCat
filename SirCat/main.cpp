//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #08
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: April 4, 2018

#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "BboxData.h"
#include "SirData.h"
#include "FindCsgo.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

struct NewData
{
	BboxData bbox; //Object to hold fresh hitbox data from game files
	SirData sir; //Object to hold fresh weapon data from game file
};

void updatePrompt(BboxData &archiveBbox, SirData &archiveSir, NewData &newData);
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
	BboxData archiveBbox(L"archiveBboxData.csv");
	SirData archiveSir(L"archiveSirData.csv");
	int menuOption = 1;

	if (!archiveBbox.getBStaticVarsInitialized() || !archiveSir.getBStaticVarsInitialized())
	{
		wcout << L"Failed to correctly retrieve archived data.\n\n\n";
		hitEnterToExit();
	}
	
	do
	{
		bool bRevertToArchive = true;

		if (menuOption == 1)
		{
			wstring steamDir;

			if (FindCsgo::inst().bFetchSteamDir(steamDir))
			{
				wcout << L"Steam installation found in directory:\n" << steamDir << endl << endl;

				if (FindCsgo::inst().bCheckCsgoInstall() //CSGO found in default Steam library
					|| FindCsgo::inst().bSearchSteamLibs()) //CSGO found in alternate Steam library
				{
					NewData newData;

					wcout << L"CS:GO installation found in directory:\n" << FindCsgo::inst().getTestDir() << endl << endl;

					if (newData.bbox.bReadModelFiles() && newData.sir.bReadWeapFile(FindCsgo::inst().getTestDir()))
					{
						wstring badRowName, badColName, badNewVal, badArchiveVal;
						bool bUpdate = true;

						bRevertToArchive = false;
						archiveBbox.readArchive();
						archiveSir.readArchive();

						if (archiveBbox.bCheckArchive(newData.bbox, badRowName, badColName, badNewVal, badArchiveVal))
							wcout << L"Data mismatch detected in " << archiveBbox.getCsvName();
						else if (archiveSir.bCheckArchive(newData.sir, badRowName, badColName, badNewVal, badArchiveVal))
							wcout << L"Data mismatch detected in " << archiveSir.getCsvName();
						else
							bUpdate = false;

						if (bUpdate)
						{
							wcout << L" for " << badRowName << L" " << badColName << L" ...\n";
							wcout << L"Value from CS:GO's game files: " << badNewVal << endl;
							wcout << L"Value from archive file: " << badArchiveVal << endl << endl;
							updatePrompt(archiveBbox, archiveSir, newData);
						}
					}
				}
			}

			if (bRevertToArchive)
			{
				wcout << L"CS:GO installation not found. Reading hitbox and weapon data from archive file.\n\n";
				archiveBbox.readArchive();
				archiveSir.readArchive();
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

void updatePrompt(BboxData &archiveBbox, SirData &archiveSir, NewData &newData)
{
	int menuOption = 0;

	do
	{
		wcout << L"Would you like to update the archive file with the new game data?\n";
		wcout << L"Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt(L"12", 2))
		{
		case 1:
			//if (archiveBbox.bWriteArchiveFile(newData.bbox) && archiveSir.bWriteArchiveFile(newData.sir))
			if (archiveSir.bWriteArchiveFile(newData.sir)) //Until BboxData::bReadModelFiles() is coded
			{
				archiveBbox.readArchive();
				archiveSir.readArchive();
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
