//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Final Project Presentation
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: May 3, 2018

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
	Data(Data &otherData) : bbox(otherData.bbox), sir(otherData.sir) {} //Call psuedo-copy constructors
};

void hitEnterToExit();
//Precondition: 
//Postcondition: 

bool bTakeOnlyOneWchar(wchar_t &character);
//Precondition: character is modifiable.
//Postcondition: Sets character to first character input

bool bAttemptFindCsgo(Data &csvData, Data &newData);
//Precondition: 
//Postcondition: 

bool bReadGameFiles(const wstring csgoDir, Data &newData);
//Precondition: 
//Postcondition: 

void compAndUpdate(Data &csvData, Data &newData);
//Precondition: 
//Postcondition: 

void listNonMatches(const Archive &archive);
//Precondition: 
//Postcondition: 

void updatePrompt(Data &csvData, const Data &newData);
//Precondition: 
//Postcondition: 

int takeOnlyOneInt(const int numValidChars, const wchar_t validChars[]);
//Precondition: numValidChars is the size of validChars.
//Postcondition: Get one and only one input character, convert it to int, and return the integer;

int wcharDigitToInt(const wchar_t wcharDigit);
//Precondition: 
//Postcondition: 

int pickModelSide(const BboxData &bbox);
//Precondition: 
//Postcondition: 

int pickModelBase(const BboxData &bbox, const wstring modelPrefix);
//Precondition: 
//Postcondition: 

int pickModelVariant(const BboxData &bbox, const wstring baseModel);
//Precondition: 
//Postcondition: 

wchar_t intDigitToWchar(const int intDigit);
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
	wchar_t startWchar;

	wcout << endl << L"SirCat will check for updated game data if CS:GO is installed.\n";
	wcout << L"Hit enter to begin... ";
	bTakeOnlyOneWchar(startWchar);
	wcout << endl << endl << L"If CS:GO is found, a helper program called Crowbar will run automatically.\n";
	wcout << L"User input will be disabled while it runs. Press Ctrl+Alt+Del if it hangs up.\n\n";
	
	do
	{
		Data csvData(wstring(L"archiveBboxData.csv"), wstring(L"archiveSirData.csv"));

		if (!csvData.bbox.getBSuccessUseCsv() || !csvData.sir.getBSuccessUseCsv())
		{
			wcout << L"Failed to correctly retrieve archived data.\n\n\n";
			hitEnterToExit();
		}

		if (menuOption == 1) //Controlled by switch in bUserMenu for re-running the program
		{
			Data newData(csvData);

			if (!bAttemptFindCsgo(csvData, newData))
				wcout << L"Using hitbox and weapon data from archive file.\n\n";

			pickModelSide(csvData.bbox);
			pickWeapon();
		}

		if (menuOption < 3) //Controlled by switch in bUserMenu for re-running the program
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

bool bAttemptFindCsgo(Data &csvData, Data &newData)
{
	bool bFoundCsgo = false;
	FindCsgo findCsgo;
	wstring steamDir;

	if (findCsgo.bFetchSteamDir(steamDir))
	{
		wcout << L"Steam installation found in directory:\n" << steamDir << endl << endl;

		if (findCsgo.bCheckCsgoInstall() //CSGO found in default Steam library
			|| findCsgo.bSearchSteamLibs()) //CSGO found in alternate Steam library
		{
			wcout << L"CS:GO installation found in directory:\n" << findCsgo.getTestDir() << endl << endl;
			wcout << L"Checking fresh CS:GO hitbox and weapon data against file archive data.\n";

			if (bReadGameFiles(findCsgo.getTestDir(), newData))
			{
				bFoundCsgo = true;
				compAndUpdate(csvData, newData);
			}
		}
		else
			wcout << L"CS:GO installation not found.\n";
	}
	else
		wcout << L"Steam installation not found.\n";

	return bFoundCsgo;
}

bool bReadGameFiles(const wstring csgoDir, Data &newData)
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

void compAndUpdate(Data &csvData, Data &newData)
{
	wcout << L"... done.";
	//Dereferences and references below allow use of dynamic_cast, which can only be used with pointers and references to classes
	//dynamic_cast ensures the result of the type conversion points to a valid, complete object of the destination pointer type
	csvData.bbox.compareArchives(*dynamic_cast<Archive *>(&newData.bbox));
	csvData.sir.compareArchives(*dynamic_cast<Archive *>(&newData.sir));

	if (csvData.bbox.getNumNonMatches() == 0 && csvData.sir.getNumNonMatches() == 0)
		wcout << L" No discrepancies detected.\n";
	else
	{
		wcout << endl;
		listNonMatches(*dynamic_cast<Archive *>(&csvData.bbox));
		listNonMatches(*dynamic_cast<Archive *>(&csvData.sir));

		updatePrompt(csvData, newData);
	}
}

void listNonMatches(const Archive &archive)
{
	if (archive.getNumNonMatches() > 0)
	{
		wcout << endl << L"Data non-match detected in " << archive.getCsvName() << ":\n";

		for (int i = 0; i < archive.getNumNonMatches(); ++i)
		{
			wcout << endl << L"Nonmatching data for " << archive.getNonMatches()[i].otherRowHeader << L" ";
			wcout << archive.getNonMatches()[i].commonColumnHeader << endl;

			if (archive.getNonMatches()[i].datum.empty())
				wcout << L"** New data--not matched in archive file **\n";
			else
				wcout << L"Value from archive file: " << archive.getNonMatches()[i].datum << endl;

			wcout << L"Value from CS:GO's game files: " << archive.getNonMatches()[i].otherDatum << endl;
		}
	}
}

void updatePrompt(Data &csvData, const Data &newData)
{
	int menuOption = 0;

	do
	{
		wcout << endl << L"Would you like to update the archive file with the new game data?\n";
		wcout << L"Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt(2, L"12"))
		{
		case 1:
			csvData.bbox = newData.bbox; //Copy assignment operator in Archive will safely copy over data in dynamic arrays
			csvData.sir = newData.sir;

			if (csvData.bbox.bWriteArchiveFile() && csvData.sir.bWriteArchiveFile())
				wcout << endl << endl << L"Archive files updated.";
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
}

int takeOnlyOneInt(const int numValidChars, const wchar_t validChars[])
{
	int integer = 0;
	wchar_t character;

	if (bTakeOnlyOneWchar(character))
	{
		for (int i = 0; i < numValidChars; ++i)
		{
			if (character == validChars[i])
			{
				integer = wcharDigitToInt(character);
				break;
			}
		}
	}

	return integer;
}

int wcharDigitToInt(const wchar_t wcharDigit)
{
	return (static_cast<int>(wcharDigit) - static_cast<int>(L'0'));
}

int pickModelSide(const BboxData &bbox)
{
	int modelIndex = -1;
	int menuOption = 0;

	do
	{
		wcout << endl << endl;
		wcout << L"1 - Counter-Terrorists\n";
		wcout << L"2 - Terrorists\n";
		wcout << L"To pick a player model for calculations, first select a team: ";

		switch (menuOption = takeOnlyOneInt(2, L"12"))
		{
		case 1:
			modelIndex = pickModelBase(bbox, wstring(L"ct"));
			break;
		case 2:
			modelIndex = pickModelBase(bbox, wstring(L"tm"));
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}

		if (modelIndex == -1)
			menuOption = 0;
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int pickModelBase(const BboxData &bbox, const wstring modelPrefix)
{
	int modelIndex;
	int menuOption;
	int numBaseModels;

	do
	{
		wstring menuModels[9];
		wstring validChars;

		numBaseModels = 0;
		menuModels[0] = L" ";
		wcout << endl << endl;

		for (int i = 0; i < bbox.getNumRows(); ++i)
		{
			if (bbox.getRowHeader(i).substr(0, menuModels[numBaseModels].length()) != menuModels[numBaseModels]
				&& bbox.getRowHeader(i).substr(0, 2) == modelPrefix) //List only CT or T models
			{
				if (bbox.getRowHeader(i).find_first_of(L'_') != bbox.getRowHeader(i).find_last_of(L'_')) //Models with no base
					menuModels[++numBaseModels] = bbox.getRowHeader(i).substr(0, bbox.getRowHeader(i).find_last_of(L'_'));
				else
					menuModels[++numBaseModels] = bbox.getRowHeader(i); //Base models

				wcout << numBaseModels << L" - " << menuModels[numBaseModels] << endl; //Build model menu
				validChars += intDigitToWchar(numBaseModels);
			}
		}

		wcout << ++numBaseModels << L" - go back to team selection\n";
		wcout << L"Now select a base model: ";
		validChars += intDigitToWchar(numBaseModels);
		menuOption = takeOnlyOneInt(numBaseModels, validChars.c_str());

		if (menuOption == 0)
			wcout << endl << endl << L"That is not a valid menu option.";
		else if (menuOption == numBaseModels)
			modelIndex = -1; //Will restart team selection in pickModelSide
		else
		{
			modelIndex = pickModelVariant(bbox, menuModels[menuOption]);

			if (modelIndex == -1)
				menuOption = 0;
		}
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int pickModelVariant(const BboxData &bbox, const wstring baseModel)
{
	int modelIndex;
	int i;
	int menuOption;
	int numVariants;

	do
	{
		wstring validChars;

		wcout << endl << endl;

		for (i = 0; i < bbox.getNumRows(); ++i)
		{
			if (bbox.getRowHeader(i).substr(0, baseModel.length()) == baseModel)
			{
				for (numVariants = 0; numVariants < bbox.getNumRows() - i; /*intentionally blank*/)
				{
					if (bbox.getRowHeader(i + numVariants).substr(0, baseModel.length()) == baseModel)
					{
						wcout << ++numVariants << L" - " << bbox.getRowHeader(i + numVariants - 1) << endl; //Build variant menu
						validChars += intDigitToWchar(numVariants);
					}
					else
						break; //Exit loop when base model changes
				}

				break;
			}
		}

		wcout << ++numVariants << L" - go back to base model selection\n";
		wcout << L"Now select a model variant: ";
		validChars += intDigitToWchar(numVariants);
		menuOption = takeOnlyOneInt(numVariants, validChars.c_str());

		if (menuOption == 0)
			wcout << endl << endl << L"That is not a valid menu option.";
	} while (menuOption == 0); //Loop until a valid menu option is input

	if (menuOption == numVariants)
		modelIndex = -1; //Restart base model selection
	else
		modelIndex = i + menuOption - 1;

	return modelIndex;
}

wchar_t intDigitToWchar(const int intDigit)
{
	return static_cast<wchar_t>(intDigit + static_cast<int>(L'0'));
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

		switch (menuOption = takeOnlyOneInt(4, L"1234"))
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
