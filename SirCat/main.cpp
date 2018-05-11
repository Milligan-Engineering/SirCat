//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Final Project Presentation
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: May 10, 2018

#include "csgo\BboxData.h"
#include "csgo\SirData.h"
#include "csgo\Archive.h"
#include "csgo\FindCsgo.h"
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <iostream>
#include <string>

using namespace std;

enum class Params { ALL, MODEL_INDEX, WEAPON_INDEX, STANCE, TAP_TYPE, DISTANCE }; //Constants to make pickCalcParams clearer

struct CalcParams
{
	int modelIndex;
	int weaponIndex;
	int stance;
	int moveSpeed;
	int tapType;
	double distance;
};

struct Data
{
	BboxData bbox;
	SirData sir;

	Data() = default; //Call default constructors for Data's member objects when an instance of a Data struct is created
	Data(wstring bboxCsvName, wstring sirCsvName) : bbox(bboxCsvName), sir(sirCsvName) {} //Call constructors that use CSV data
	Data(Data &otherData) : bbox(otherData.bbox), sir(otherData.sir) {} //Call psuedo-copy constructors
	~Data() = default;
};

void hitEnterToExit();
//Precondition: The console is ready to display a message then receive user input.
//Postcondition: After the user hits enter, the program will exit.

bool bTakeOnlyOneWchar(wchar_t &character);
//Precondition: character is modifiable and the console is ready to receive user input.
//Postcondition: Sets character to first character input from the user.
	//Returns true if the user input only one character before hitting enter, and false otherwise.

bool bAttemptFindCsgo(Data &csvData, Data &newData);
//Precondition: csvData and newData are modifiable, csvData is populated with game data, and newData can receive fresh data.
//Postcondition: Attempts to locate an installation of CS:GO on the users computer.
	//If found, newData is populated with game data, csvData and newData are compared and option is given to update the CSV file.
	//Returns true if CS:GO is found and the game data is successfully parsed into newData, and false otherwise.

bool bReadGameFiles(const wstring csgoDir, Data &newData);
//Precondition: csgoDir is a directory containing a valid install of CS:GO; newData is modifiable and can receive fresh data.
//Postcondition: Invokes programs to unpack, decompile, and read game data into newData, giving feedback for success/failure.
	//Returns true if the game data is successfully parsed into newData, and false otherwise with accompanying error message.

void compAndUpdate(Data &csvData, Data &newData);
//Precondition: csvData and newData are modifiable and populated with game data.
//Postcondition: csvData and newData are compared, the results are displayed, and option is given to update the CSV file.

void listNonMatches(const Archive &archive);
//Precondition: archive is a properly casted polymorphic parent reference of a SirData or BboxData object.
//Postcondition: The non-matching data array from archive is parsed and non-matches are displayed.

void updatePrompt(Data &csvData, const Data &newData);
//Precondition: csvData and newData are modifiable and populated with game data.
	//The console is ready to display a message then receive user input.
//Postcondition: Option given to update the CSV file with fresh game data; newData's data is copied into csvData if input is 1.

int takeOnlyOneInt(const int numValidChars, const wchar_t validChars[]);
//Precondition: numValidChars is the size of validChars and the console is ready to receive user input.
//Postcondition: If the user input only one character before hitting enter, returns that character as an integer and 0 otherwise.

int wcharDigitToInt(const wchar_t wcharDigit);
//Precondition: wcharDigit is a digit (0-9).
//Postcondition: Converts wcharDigit to an integer and returns it.

void pickCalcParams(CalcParams &calcParams, const Data &csvData, const Params whichParams = Params::ALL);
//Precondition: 
//Postcondition: 

int pickModelSide(const BboxData &bbox);
//Precondition: bbox is populated with game data and the console is ready to display a message then receive user input.
//Postcondition: Loops until model side, base, and variant are selected and returns the index of that model in bbox's data array.

int pickModelBase(const BboxData &bbox, const wstring modelPrefix);
//Precondition: bbox is populated with game data and the console is ready to display a message then receive user input.
//Postcondition: Loops until model base and variant are selected and returns the index of that model in bbox's data array.

int pickModelVariant(const BboxData &bbox, const wstring baseModel);
//Precondition: bbox is populated with game data and the console is ready to display a message then receive user input.
//Postcondition: Loops until model variant is selected and returns the index of that model in bbox's data array.

wchar_t intDigitToWchar(const int intDigit);
//Precondition: intDigit is a digit (0-9).
//Postcondition: Converts intDigit to a character and returns it.

int pickWeapon(const SirData &sir);
//Precondition: sir is populated with game data and the console is ready to display a message then receive user input.
//Postcondition: To continue, user must pick a weapon, whose index in sir's data array will be returned.

void calcIdealFreq(const CalcParams calcParams, const Data &csvData);
//Precondition: 
//Postcondition: 

bool bUserMenu(int &menuOption, CalcParams &calcParams, const Data &csvData);
//Precondition: menuOption and calcParams are modifiable and csvData is populated with game data.
	//The console is ready to display a message then receive user input.
//Postcondition: menuOption is updated according to user input in response to a program menu.
	//calcParams is updated according to user inputs if the option is chosen by the user.
	//Returns true if the program should continue or false if the program should exit.

int main()
{
	CalcParams calcParams;
	Data *csvData = nullptr;
	int menuOption = 1;
	wchar_t startWchar;

	wcout << endl << L"SirCat will check for updated game data if CS:GO is installed.\n";
	wcout << L"Hit enter to begin... ";
	bTakeOnlyOneWchar(startWchar);
	wcout << endl << endl << L"If CS:GO is found, a helper program called Crowbar will run automatically.\n";
	wcout << L"User input will be disabled while it runs. Press Ctrl+Alt+Del if it hangs up.\n\n";
	
	do
	{
		if (menuOption == 1) //Controlled by switch in bUserMenu for re-running the program
		{
			Data *newData = nullptr;

			if (csvData != nullptr)
				delete csvData;

			csvData = new Data(wstring(L"archiveBboxData.csv"), wstring(L"archiveSirData.csv"));

			if (!csvData->bbox.getBSuccessUseCsv() || !csvData->sir.getBSuccessUseCsv())
			{
				delete csvData;
				hitEnterToExit();
			}

			newData = new Data(*csvData);

			if (!bAttemptFindCsgo(*csvData, *newData))
				wcout << L"Using hitbox and weapon data from archive file.\n\n";

			delete newData;
			pickCalcParams(calcParams, *csvData);
		}

		calcIdealFreq(calcParams, *csvData);
	} while (bUserMenu(menuOption, calcParams, *csvData));

	delete csvData;

	return 0;
}

void hitEnterToExit()
{
	wchar_t exitWchar;

	wcout << L"Failed to correctly retrieve archived data.\n\n\n";
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
			wcout << endl << L"Non-matching data for " << archive.getNonMatches()[i].otherRowHeader << L" ";
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

void pickCalcParams(CalcParams &calcParams, const Data &csvData, const Params whichParams)
{
	if (whichParams == Params::ALL || whichParams == Params::MODEL_INDEX)
		calcParams.modelIndex = pickModelSide(csvData.bbox);

	if (whichParams == Params::ALL || whichParams == Params::WEAPON_INDEX)
		calcParams.weaponIndex = pickWeapon(csvData.sir);

	//if (whichParams == Params::ALL || whichParams == Params::STANCE)

	//if (whichParams == Params::ALL || whichParams == Params::TAP_TYPE)

	if (whichParams == Params::ALL || whichParams == Params::DISTANCE)
	{
		wstring distance = L"not empty";

		do
		{
			size_t decimalPos;

			if (distance.empty())
			{
				wcout << endl << endl << L"That distance is not valid.";
				wcout << endl << L"Requires 0.01 <= distance <= 300.00 and 2 or fewer decimal places.";
			}

			wcout << endl << endl << L"Input distance from target in feet (0.01-300.00): ";
			getline(wcin, distance);

			for (size_t i = 0; i < distance.length(); ++i)
			{
				if (iswdigit(static_cast<wint_t>(distance[i])) == 0 && distance[i] != L'.')
				{
					distance.clear(); //Non-number characters in input distance
					break;
				}
			}

			decimalPos = distance.find_first_of(L'.');

			if (distance.length() == decimalPos + 1)
				distance += L'0';

			if ((distance.find_last_of(L'.') != decimalPos || distance.substr(decimalPos + 1).length() > 2)
				&& decimalPos != distance.npos || stod(distance) < 0.01 || stod(distance) > 300.00)
				distance.clear(); //Invalid number format for input distance
		} while (distance.empty());

		calcParams.distance = stod(distance) * 0.3048; //Convert to meters
	}
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

		for (i = 0; i < bbox.getNumRows(); ++i) //Find first baseModel variants
		{
			if (bbox.getRowHeader(i).substr(0, baseModel.length()) == baseModel)
			{
				numVariants = 0;

				while(numVariants < bbox.getNumRows() - i)
				{
					if (bbox.getRowHeader(i + numVariants).substr(0, baseModel.length()) != baseModel)
						break; //Exit loop if base model changes
					
					wcout << ++numVariants << L" - " << bbox.getRowHeader(i + numVariants - 1) << endl; //Build variant menu
					validChars += intDigitToWchar(numVariants);
				}

				break; //All baseModel variants found
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

int pickWeapon(const SirData &sir)
{
	int weaponIndex = -1;
	int i = 0;
	int menuOption = 0;

	do
	{
		do
		{
			int displayMoreNum = 0;
			int menuNumber;
			wstring validChars;

			wcout << endl << endl;

			for (menuNumber = 1; menuNumber <= 8 && i < sir.getNumRows(); ++menuNumber) //List weapons 8 at a time until
			{
				wcout << menuNumber << L" - " << sir.getRowHeader(i) << endl; //Build weapon menu
				validChars += intDigitToWchar(menuNumber);
				++i;
			}

			if (i != sir.getNumRows()) //Option to display next weapon menu if unlisted weapons remain
			{
				displayMoreNum = menuNumber;
				validChars += intDigitToWchar(menuNumber);
				wcout << menuNumber++ << L" - display more weapons\n";
			}

			wcout << L"Select a weapon to use, or display more weapons to choose from: ";
			menuOption = takeOnlyOneInt(--menuNumber, validChars.c_str());

			if (menuOption == 0)
			{
				wcout << endl << endl << L"That is not a valid menu option.";
				i -= menuNumber;
			}
			else if (menuOption != displayMoreNum)
			{
				if (displayMoreNum == 0)
					weaponIndex = i - 1 - menuNumber + menuOption;
				else
					weaponIndex = i - menuNumber + menuOption;
			}
		} while (menuOption == 0); //Loop until a valid menu option is input
	} while (weaponIndex == -1);

	return weaponIndex;
}

void calcIdealFreq(const CalcParams calcParams, const Data &csvData)
{
	enum Sir { CYCLE_TIME, INACCURACY, INACCURACY_FIRE, RECOVERY_TIME, SPREAD, NUM_STATS };

	const double targetInaccuracy = stod(csvData.bbox.getDatum(calcParams.modelIndex, csvData.bbox.getNumColumns() - 1))
		* 0.0254 / 0.001 / calcParams.distance; //Convert from Hammer units to meters then use effective range formula
	const wchar_t *statStrings[Sir::NUM_STATS] = { L"cycletime", L"inaccuracy stand", L"inaccuracy fire",
		L"recovery time stand", L"spread" };
	double inaccuracy;
	double newInaccuracy;
	double sirStats[Sir::NUM_STATS];
	double tapInterval;
	int j[Sir::NUM_STATS];
	////////////////////////////////////////////////////////
	wcout << targetInaccuracy << endl; /////////////////////////////////////////////////

	for (int stat = 0; stat < static_cast<int>(Sir::NUM_STATS); ++stat)
	{
		j[stat] = csvData.sir.fetchColumnIndex(wstring(statStrings[stat]));

		if (j[stat] == -1)
			hitEnterToExit();

		sirStats[stat] = stod(csvData.sir.getDatum(calcParams.weaponIndex, j[stat]));
	}

	tapInterval = sirStats[Sir::CYCLE_TIME];

	do
	{
		inaccuracy = sirStats[Sir::SPREAD] + sirStats[Sir::INACCURACY];
		wcout << inaccuracy << endl; //////////////////////////////////////////////////////////
		newInaccuracy = inaccuracy * pow(0.1, tapInterval / sirStats[Sir::RECOVERY_TIME]) + sirStats[Sir::INACCURACY_FIRE];
		wcout << newInaccuracy << endl; //////////////////////////////////////////////////////////

		if (newInaccuracy > targetInaccuracy && round(newInaccuracy * 10000) - round(inaccuracy * 10000) < 5)
		{
			wcout << endl << endl << L"Weapon is not accurate enough to have 100% tapping accuracy with current criteria.";
			break;
		}

		do
		{
			inaccuracy = newInaccuracy;
			newInaccuracy = inaccuracy * pow(0.1, tapInterval / sirStats[Sir::RECOVERY_TIME]) + sirStats[Sir::INACCURACY_FIRE];
			wcout << newInaccuracy << endl; //////////////////////////////////////////////////////////
		} while (newInaccuracy > inaccuracy); //newInaccuracy will eventually dip below old inaccuracy due to increasing decay

		if (newInaccuracy <= targetInaccuracy)
		{
			wcout << endl << endl << L"Ideal tap-fire interval: " << tapInterval << " seconds\n\n\n";
			break;
		}

		tapInterval += 1.0 / 128.0; //Increment by one tick
	} while (true);
}

bool bUserMenu(int &menuOption, CalcParams &calcParams, const Data &csvData)
{
	bool bContinue = true;

	do
	{
		wcout << endl << endl;
		wcout << L"1 - start over for a fresh calculation from actual game data\n";
		wcout << L"2 - modify existing selections and calculate again\n";
		wcout << L"3 - exit the program\n";
		wcout << L"Please enter a choice from the preceding menu options: ";

		switch (menuOption = takeOnlyOneInt(3, L"123"))
		{
		case 1:
			wcout << endl << endl;
			break;
		case 2:
			do
			{
				int subMenuOption = 0;

				wcout << endl << endl;
				wcout << L"1 - modify model selection\n";
				wcout << L"2 - modify weapon selection\n";
				wcout << L"3 - modify stance\n";
				wcout << L"4 - modify tapping type\n";
				wcout << L"5 - modify distance\n";
				wcout << L"6 - no more modifications; perform the calculation\n";
				wcout << L"Please enter a choice from the preceding menu options: ";
				subMenuOption = takeOnlyOneInt(6, L"123456");

				if (subMenuOption == 6)
					break;
				
				if (subMenuOption != 0)
					pickCalcParams(calcParams, csvData, static_cast<Params>(subMenuOption));
				else
					wcout << endl << endl << L"That is not a valid menu option.";
			} while (true);

			break;
		case 3:
			wcout << endl;
			bContinue = false;
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}
	} while (menuOption == 0); //Loop until a valid menu option is input

	return bContinue;
}
