//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #08
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 28, 2018

#include "BboxData.h"
#include "SirData.h"
#include "FindCsgo.h"
#include <cstdlib>
#include <iostream>

using namespace std;

void updatePrompt(SirData &archiveSir, SirData &newSir, BboxData &archiveBbox, BboxData &newBbox);
//Precondition: 
//Postcondition: 

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
	SirData archiveSir("archiveSirData.csv");
	BboxData archiveBbox("archiveBboxData.csv");
	int menuOption = 1;

	if (!archiveSir.getBStaticVarsInitialized() || !archiveBbox.getBStaticVarsInitialized())
	{
		cout << "Failed to correctly retrieve archived data.\n\n\n";

		hitEnterToExit();
	}
	
	do
	{
		bool bRevertToArchive = true;

		if (menuOption == 1)
		{
			if (FindCsgo::inst().bFetchSteamDir())
			{
				if (FindCsgo::inst().bCheckCsgoInstall() //CSGO found in default Steam library
					|| FindCsgo::inst().bSearchSteamLibs()) //CSGO found in alternate Steam library
				{
					SirData newSir; //New object to hold fresh weapon data from game file
					BboxData newBbox; //New object to hold fresh hitbox data from game files

					cout << "CS:GO installation found in directory:\n" << FindCsgo::inst().getTestDir() << endl << endl;

					if (newSir.bReadWeapFile(FindCsgo::inst().getTestDir()) && newBbox.bReadModelFiles())
					{
						bRevertToArchive = false;
						archiveSir.readArchive();
						archiveBbox.readArchive();

						if (archiveSir.bCheckArchive(newSir) || archiveBbox.bCheckArchive(newBbox))
							updatePrompt(archiveSir, newSir, archiveBbox, newBbox);
					}
				}
			}

			if (bRevertToArchive)
			{
				cout << "CS:GO installation not found. Reading hitbox and weapon data from archive file.\n\n";

				archiveSir.readArchive();
				archiveBbox.readArchive();
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

void updatePrompt(SirData &archiveSir, SirData &newSir, BboxData &archiveBbox, BboxData &newBbox)
{
	int menuOption = 0;

	do
	{
		cout << "Would you like to update the archive file with the new game data?\n";
		cout << "Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt("12", 2))
		{
		case 1:
			//if (archiveSir.bWriteArchiveFile(newSir) && archiveBbox.bWriteArchiveFile(newBbox))
			if (archiveSir.bWriteArchiveFile(newSir)) //Until BboxData::bReadModelFiles() is coded
			{
				archiveSir.readArchive();
				archiveBbox.readArchive();

				cout << endl << "Archive file updated." << endl << endl;
			}
			else
				cout << endl << "Failed to update archive file." << endl << endl;

			break;
		case 2:
			break;
		default:
			cout << endl << "That is not a valid menu option.\n\n";
		}
	} while (!static_cast<bool>(menuOption));
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

void hitEnterToExit()
{
	char exitChar;

	cout << endl << "Hit enter to exit: ";

	bTakeOnlyOneChar(exitChar);
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
