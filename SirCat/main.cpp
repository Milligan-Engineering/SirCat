//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #10
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: April 22, 2018

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
	Data(wstring bboxCsvName, wstring sirCsvName) : bbox(bboxCsvName), sir(sirCsvName) {} //Call other constructors
};

bool bReadGameFiles(Data &newData, const wstring csgoDir);
//Precondition: 
//Postcondition:

void listNonMatches(Archive::NonMatch *nonMatches, int numNonMatches);
//Precondition: 
//Postcondition:

void updatePrompt(Data &archiveData, Data &newData);
//Precondition: 
//Postcondition: 

bool bUserMenu(int &menuOption);
//Precondition: menuOption is modifiable.
//Postcondition: menuOption is updated according to user input in response to a program menu.
	//Returns true if the program should continue or false if the program should exit.

int takeOnlyOneInt(const wchar_t validChars[], const int numValidChars);
//Precondition: numValidChars is the size of validChars.
//Postcondition: Get one and only one input character, convert it to int, and return the integer;

bool bTakeOnlyOneWchar(wchar_t &character);
//Precondition: character is modifiable.
//Postcondition: Sets character to first character input 

int WcharDigitToInt(const wchar_t wcharDigit);
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
	int menuOption = 1;

	do
	{
		bool bRevertToArchive = true;
		Data csvData(static_cast<wstring>(L"archiveBboxData.csv"), static_cast<wstring>(L"archiveSirData.csv"));

		if (!csvData.bbox.getBSuccessUseCsv() || !csvData.sir.getBSuccessUseCsv())
		{
			wcout << L"Failed to correctly retrieve archived data.\n\n\n";
			hitEnterToExit();
		}

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
					Archive *pArchiveChild = &csvData.bbox;///////////////////////////////////////////////////////////////
														   ///////////////////////////////////////////////////////////////
					newData.bbox.temp(pArchiveChild, true, true, true, true, true);///////////////////////////////////////////////////////////////
					pArchiveChild = &csvData.sir;///////////////////////////////////////////////////////////////
					newData.sir.temp(pArchiveChild, true, true, true, true, true);///////////////////////////////////////////////////////////////
					wcout << L"CS:GO installation found in directory:\n" << findCsgo.getTestDir() << endl << endl;
					wcout << L"Checking fresh CS:GO hitbox and weapon data against file archive data.\n";

					if (bReadGameFiles(newData, findCsgo.getTestDir()))
					{
						bool bUpdate = true;
						Archive::NonMatch *nonMatchesBbox = nullptr;
						Archive::NonMatch *nonMatchesSir = nullptr;
						int numNonMatchesBbox = 0;
						int numNonMatchesSir = 0;

						bRevertToArchive = false;
						wcout << L"... done.";
						numNonMatchesBbox = csvData.bbox.compareArchives(&newData.bbox, false, &nonMatchesBbox);
						numNonMatchesSir = csvData.sir.compareArchives(&newData.sir, false, &nonMatchesSir);

						if (numNonMatchesBbox == 0 && numNonMatchesSir == 0)
						{
							wcout << L" No discrepancies detected.\n\n";
							bUpdate = false;
						}

						if (bUpdate)
						{
							wcout << endl;

							if (numNonMatchesBbox > 0)
							{
								wcout << endl << L"Data non-match detected in " << csvData.bbox.getCsvName() << ":\n";
								listNonMatches(nonMatchesBbox, numNonMatchesBbox);
							}

							if (numNonMatchesSir > 0)
							{
								wcout << endl << L"Data non-match detected in " << csvData.sir.getCsvName() << ":\n";
								listNonMatches(nonMatchesSir, numNonMatchesSir);
							}

							updatePrompt(csvData, newData);

							if (nonMatchesBbox != nullptr)
							{
								delete[] nonMatchesBbox;
								nonMatchesBbox = nullptr;
							}

							if (nonMatchesSir != nullptr)
							{
								delete[] nonMatchesSir;
								nonMatchesSir = nullptr;
							}
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
			wcout << endl << L"Decompiling hitbox binaries failed.\n";
	}
	else
		wcout << endl << L"Unpacking hitbox files from CS:GO VPK failed.\n";

	return bSuccess;
}

void listNonMatches(Archive::NonMatch *nonMatches, int numNonMatches)
{
	for (int i = 0; i < numNonMatches; ++i)
	{
		wcout << L"Nonmatching data for " << nonMatches[i].otherRowHeader << L" " << nonMatches[i].commonColumnHeader << endl;

		if (nonMatches[i].datum.empty())
			wcout << L"** New data--not matched in archive file **\n";
		else
			wcout << L"Value from archive file: " << nonMatches[i].datum << endl;

		wcout << L"Value from CS:GO's game files: " << nonMatches[i].otherDatum << endl << endl;
	}
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
			if (archiveData.bbox.bWriteArchiveFile(static_cast<wstring>(L"archiveBboxData.csv"))
				&& archiveData.sir.bWriteArchiveFile(static_cast<wstring>(L"archiveSirData.csv")))
				wcout << endl << endl << L"Archive files updated." << endl << endl;
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

int WcharDigitToInt(const wchar_t wcharDigit)
{
	return (static_cast<int>(wcharDigit) - static_cast<int>(L'0'));
}

void hitEnterToExit()
{
	wchar_t exitWchar;

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
