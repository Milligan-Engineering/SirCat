//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Final Project Presentation
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: April 30, 2018

#include "csgo\BboxData.h"
#include "csgo\SirData.h"
#include "csgo\Archive.h"
#include "csgo\FindCsgo.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

struct Data
{
	BboxData bbox;
	SirData sir;

	Data() = default; //Call default constructors for Data's member objects when an instance of a Data struct is created
	Data(wstring bboxCsvName, wstring sirCsvName) : bbox(bboxCsvName), sir(sirCsvName) {} //Call constructors that use CSV data
	Data(Data &otherData) : bbox(otherData.bbox), sir(otherData.sir) {} //Call copy constructors
};

void hitEnterToExit();
//Precondition: 
//Postcondition: 

bool bTakeOnlyOneWchar(wchar_t &character);
//Precondition: character is modifiable.
//Postcondition: Sets character to first character input 

bool bReadGameFiles(Data &newData, const wstring csgoDir);
//Precondition: 
//Postcondition: 

void listNonMatches(const Archive *const archive);
//Precondition: 
//Postcondition: 

bool bUpdatePrompt(Data &newData);
//Precondition: 
//Postcondition: 

int takeOnlyOneInt(const wchar_t validChars[], const int numValidChars);
//Precondition: numValidChars is the size of validChars.
//Postcondition: Get one and only one input character, convert it to int, and return the integer;

int WcharDigitToInt(const wchar_t wcharDigit);
//Precondition: 
//Postcondition: 

int pickModelSide(const BboxData &bbox);
//Precondition: 
//Postcondition: 

int pickModel(const wstring modelPrefix, const BboxData &bbox);
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

bool bUserMenu(int &menuOption);
//Precondition: menuOption is modifiable.
//Postcondition: menuOption is updated according to user input in response to a program menu.
//Returns true if the program should continue or false if the program should exit.

int main()
{
	int menuOption = 1;
	
	do
	{
		bool bRevertToArchive = true;
		bool bUseCsvData = true;
		Data csvData(wstring(L"archiveBboxData.csv"), wstring(L"archiveSirData.csv"));

		if (!csvData.bbox.getBSuccessUseCsv() || !csvData.sir.getBSuccessUseCsv())
		{
			wcout << L"Failed to correctly retrieve archived data.\n\n\n";
			hitEnterToExit();
		}

		if (menuOption == 1)
		{
			FindCsgo findCsgo;
			wstring steamDir;
			Data newData(csvData);

			if (findCsgo.bFetchSteamDir(steamDir))
			{
				wcout << L"Steam installation found in directory:\n" << steamDir << endl << endl;

				if (findCsgo.bCheckCsgoInstall() //CSGO found in default Steam library
					|| findCsgo.bSearchSteamLibs()) //CSGO found in alternate Steam library
				{
					wcout << L"CS:GO installation found in directory:\n" << findCsgo.getTestDir() << endl << endl;
					wcout << L"Checking fresh CS:GO hitbox and weapon data against file archive data.\n";

					if (bReadGameFiles(newData, findCsgo.getTestDir()))
					{
						bRevertToArchive = false;
						wcout << L"... done.";
						csvData.bbox.compareArchives(static_cast<Archive *>(&newData.bbox));
						csvData.sir.compareArchives(static_cast<Archive *>(&newData.sir));

						if (csvData.bbox.getNumNonMatches() == 0 && csvData.sir.getNumNonMatches() == 0)
							wcout << L" No discrepancies detected.\n\n";
						else
						{
							wcout << endl;
							listNonMatches(static_cast<Archive *>(&csvData.bbox));
							listNonMatches(static_cast<Archive *>(&csvData.sir));
							if (bUpdatePrompt(newData))
								bUseCsvData = false;
						}
					}
				}
				else
					wcout << L"CS:GO installation not found.\n";
			}
			else
				wcout << L"Steam installation not found.\n";

			if (bRevertToArchive)
				wcout << L"Reading hitbox and weapon data from archive file.\n\n";

			if (bUseCsvData)
			{
				pickModelSide(csvData.bbox);
				pickWeapon();
			}
			else
			{
				pickModelSide(newData.bbox);
				pickWeapon();
			}
		}

		if (menuOption < 3)
			bUserModifyData();

		//******Ask user to enter a distance******
		calcIdealFreq();
	} while (bUserMenu(menuOption));

	return 0;
}

void hitEnterToExit()
{
	wchar_t exitWchar;

	wcout << endl << L"Hit enter to exit: ";
	bTakeOnlyOneWchar(exitWchar);
	wcout << endl;
	exit(1);
}

bool bTakeOnlyOneWchar(wchar_t &character)
{
	bool bValidInput = false;
	bool bFirstChar = true;
	wchar_t input;

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

bool bReadGameFiles(Data &newData, const wstring csgoDir)
{
	bool bSuccess = false;

	wcout << L"... Unpacking hitbox binaries from CS:GO VPK with HLExtract ...\n";

	if (newData.bbox.bUnpackModels(csgoDir))
	{
		wcout << L"... Decompiling hitbox binaries with Crowbar ...\n";

		if (newData.bbox.bDecompileModels())
		{
			wcout << L"... Reading decompiled hitbox files ...\n";

			if (newData.bbox.bReadModelFiles())
			{
				wcout << L"... Reading weapon data from CS:GO items_game.txt ...\n";

				if (newData.sir.bReadWeapFile(csgoDir))
					bSuccess = true;
				else
					wcout << endl << L"Reading weapon data from CS:GO items_game.txt failed ...\n";
			}
			else
				wcout << endl << L"Reading decompiled hitbox files failed.\n";
		}
		else
		{
			newData.bbox.bReadModelFiles(true);
			wcout << endl << L"Decompiling hitbox binaries failed.\n";
		}
	}
	else
		wcout << endl << L"Unpacking hitbox files from CS:GO VPK failed.\n";

	return bSuccess;
}

void listNonMatches(const Archive *const archive)
{
	if (archive->getNumNonMatches() > 0)
	{
		wcout << endl << L"Data non-match detected in " << archive->getCsvName() << ":\n";

		for (int i = 0; i < archive->getNumNonMatches(); ++i)
		{
			wcout << L"Nonmatching data for " << archive->getNonMatches()[i].otherRowHeader << L" "
				<< archive->getNonMatches()[i].commonColumnHeader << endl;

			if (archive->getNonMatches()[i].datum.empty())
				wcout << L"** New data--not matched in archive file **\n";
			else
				wcout << L"Value from archive file: " << archive->getNonMatches()[i].datum << endl;

			wcout << L"Value from CS:GO's game files: " << archive->getNonMatches()[i].otherDatum << endl << endl;
		}
	}
}

bool bUpdatePrompt(Data &newData)
{
	bool bUpdated = false;
	int menuOption = 0;

	do
	{
		wcout << L"Would you like to update the archive file with the new game data?\n";
		wcout << L"Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt(L"12", 2))
		{
		case 1:
			if (newData.bbox.bWriteArchiveFile(wstring(L"archiveBboxData.csv"))
				&& newData.sir.bWriteArchiveFile(wstring(L"archiveSirData.csv")))
			{
				wcout << endl << endl << L"Archive files updated.";
				bUpdated = true;
			}
			else
				wcout << endl << endl << L"Failed to update archive files.";

			break;
		case 2:
			wcout << endl << endl;
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.\n\n";
		}
	} while (menuOption == 0);

	return bUpdated;
}

int takeOnlyOneInt(const wchar_t validChars[], const int numValidChars)
{
	int integer = 0;
	wchar_t character;

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

int WcharDigitToInt(const wchar_t wcharDigit)
{
	return (static_cast<int>(wcharDigit) - static_cast<int>(L'0'));
}

int pickModelSide(const BboxData &bbox)
{
	int modelIndex;
	int menuOption = 0;

	do
	{
		wcout << endl << endl;
		wcout << L"1 - Counter-Terrorists\n";
		wcout << L"2 - Terrorists\n";
		wcout << L"To pick a player model for calculations, first select a team: ";

		switch (menuOption = takeOnlyOneInt(L"12", 2))
		{
		case 1:
			modelIndex = pickModel(wstring(L"ct"), bbox);
			break;
		case 2:
			modelIndex = pickModel(wstring(L"tm"), bbox);
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}

		if (modelIndex == 0)
			menuOption = 0;
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int pickModel(const wstring modelPrefix, const BboxData &bbox)
{
	int modelIndex;
	int numMenuModels;
	int menuOption = 0;

	do
	{
		wstring menuModel = L" ";
		wstring validChars;

		numMenuModels = 0;
		wcout << endl << endl;

		for (int i = 0; i < bbox.getNumRows(); ++i)
		{
			if (bbox.getRowHeader(i).substr(0, menuModel.length()) != menuModel //Don't list model variants yet
				&& bbox.getRowHeader(i).substr(0, 2) == modelPrefix)
			{
				if (bbox.getRowHeader(i).find_first_of(L'_') != bbox.getRowHeader(i).find_last_of(L'_'))
					menuModel = bbox.getRowHeader(i).substr(0, bbox.getRowHeader(i).find_last_of(L'_')); //Models with no base
				else
					menuModel = bbox.getRowHeader(i);

				wcout << ++numMenuModels << L" - " << menuModel << endl; //Build model menu
				validChars += static_cast<wchar_t>(numMenuModels + static_cast<int>(L'0'));
			}
		}

		wcout << ++numMenuModels << L" - go back to team selection\n";
		wcout << L"Now select a base model: ";
		validChars += static_cast<wchar_t>(numMenuModels + static_cast<int>(L'0'));
		menuOption = takeOnlyOneInt(validChars.c_str(), numMenuModels);

		if (menuOption == 0)
			wcout << endl << endl << L"That is not a valid menu option.";
		else
			modelIndex = menuOption;
	} while (menuOption == 0); //Loop until a valid menu option is input

	if (modelIndex == numMenuModels)
		modelIndex = 0; //Will restart team selection in pickModelSide

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

bool bUserMenu(int &menuOption)
{
	bool bContinue = true;

	do
	{
		wcout << endl << endl;
		wcout << L"1 - start over for a fresh calculation, starting from actual game data\n";
		wcout << L"2 - modify hitbox and weapon data for another calculation\n";
		wcout << L"3 - pick distance for another calculation with the same hitbox and weapon data\n";
		wcout << L"4 - exit the program\n";
		wcout << L"Please enter a choice from the preceding menu options: ";

		switch (menuOption = takeOnlyOneInt(L"1234", 4))
		{
		case 1:
			wcout << endl << endl;
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			wcout << endl;
			bContinue = false;
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}
	} while (menuOption == 0); //Loop until a valid menu option is input

	return bContinue;
}
