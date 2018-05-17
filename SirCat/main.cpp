//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: May 16, 2018

#include "csgo\bbox\BboxData.h"
#include "csgo\sir\SirData.h"
#include "csgo\Archive.h"
#include "csgo\Calc.h"
#include "csgo\FindCsgo.h"
#include <cstdlib>
#include <cwctype>
#include <iostream>
#include <string>

namespace sircat {

using csgo::bbox::BboxData;
using csgo::sir::SirData;
using csgo::Archive;
using csgo::Calc;
using csgo::FindCsgo;
using std::exit;
using std::iswdigit;
using std::endl;
using std::wcin;
using std::wcout;
using std::stod;
using std::wstring;

struct CsgoData
{
	BboxData bboxData;
	SirData sirData;

	CsgoData() = default;
	CsgoData(wstring bboxCsvName, wstring sirCsvName) : bboxData(bboxCsvName), sirData(sirCsvName) {}
	CsgoData(CsgoData &otherData) : bboxData(otherData.bboxData), sirData(otherData.sirData) {} //Call psuedo-copy constructors
	~CsgoData() = default;
};

void hitEnterToExit();
//Precondition: The console is ready to display a message then receive user input
//Postcondition: After the user hits enter, the program will exit

bool bTakeOnlyOneWchar(wchar_t &character);
//Precondition: character is modifiable and the console is ready to receive user input
//Postcondition: Sets character to first character input from the user
	//Returns true if the user input only one character before hitting enter, and false otherwise

bool bAttemptFindCsgo(CsgoData &csvData, CsgoData &newData);
//Precondition: csvData and newData are modifiable, csvData is populated with game data, and newData can receive fresh data
//Postcondition: Attempts to locate an installation of CS:GO on the users computer
	//If found, newData is populated with game data, csvData and newData are compared and option is given to update the CSV file
	//Returns true if CS:GO is found and the game data is successfully parsed into newData, and false otherwise

bool bReadGameFiles(const wstring csgoDir, CsgoData &newData);
//Precondition: csgoDir is a directory containing a valid install of CS:GO; newData is modifiable and can receive fresh data
//Postcondition: Invokes programs to unpack, decompile, and read game data into newData, giving feedback for success/failure
	//Returns true if the game data is successfully parsed into newData, and false otherwise with accompanying error message

void compAndUpdate(CsgoData &csvData, CsgoData &newData);
//Precondition: csvData and newData are modifiable and populated with game data
//Postcondition: csvData and newData are compared, the results are displayed, and option is given to update the CSV file

void listNonMatches(const Archive &archive);
//Precondition: archive is a properly casted polymorphic parent reference of a SirData or BboxData object
//Postcondition: The non-matching data array from archive is parsed and non-matches are displayed

void updatePrompt(CsgoData &csvData, const CsgoData &newData);
//Precondition: csvData and newData are modifiable and populated with game data
	//The console is ready to display a message then receive user input
//Postcondition: Option given to update the CSV file with fresh game data; newData's data is copied into csvData if input is 1

int takeOnlyOneInt(const int numValidChars, const wchar_t validChars[]);
//Precondition: numValidChars is the size of validChars and the console is ready to receive user input
//Postcondition: If the user input only one character before hitting enter, returns that character as an integer and 0 otherwise

int wcharDigitToInt(const wchar_t wcharDigit);
//Precondition: wcharDigit is a digit (0-9)
//Postcondition: Converts wcharDigit to an integer and returns it

void pickCalcParams(Calc::Params &calcParams, const CsgoData &csvData, const Calc::WhichParam whichParams = Calc::WhichParam::ALL);

int pickModelSide(const BboxData &bboxData);
//Precondition: bboxData is populated with game data and the console is ready to display a message then receive user input
//Postcondition: Loops until model side, base, and variant are selected and returns index of that model in bboxData's data array

int pickModelBase(const BboxData &bboxData, const wstring modelPrefix);
//Precondition: bboxData is populated with game data and the console is ready to display a message then receive user input
//Postcondition: Loops until model base and variant are selected and returns the index of that model in bboxData's data array

int pickModelVariant(const BboxData &bboxData, const wstring baseModel);
//Precondition: bboxData is populated with game data and the console is ready to display a message then receive user input
//Postcondition: Loops until model variant is selected and returns the index of that model in bboxData's data array

wchar_t intDigitToWchar(const int intDigit);
//Precondition: intDigit is a digit (0-9)
//Postcondition: Converts intDigit to a character and returns it

int pickWeapon(const SirData &sirData);
//Precondition: sirData is populated with game data and the console is ready to display a message then receive user input
//Postcondition: To continue, user must pick a weapon, whose index in sirData's data array will be returned

double pickDistance();

void calcIdealFreq(const Calc::Params &calcParams, const CsgoData &csvData);

bool bUserMenu(int &menuOption, Calc::Params &calcParams, const CsgoData &csvData);
//Precondition: menuOption and calcParams are modifiable and csvData is populated with game data
	//The console is ready to display a message then receive user input
//Postcondition: menuOption is updated according to user input in response to a program menu
	//calcParams is updated according to user inputs if the option is chosen by the user
	//Returns true if the program should continue or false if the program should exit

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

bool bAttemptFindCsgo(CsgoData &csvData, CsgoData &newData)
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

bool bReadGameFiles(const wstring csgoDir, CsgoData &newData)
{
	bool bSuccess = false;

	wcout << L"... Unpacking hitbox binaries from CS:GO VPK with HLExtract ...\n";

	if (newData.bboxData.bUnpackModels(csgoDir))
	{
		wcout << L"... Decompiling hitbox binaries with Crowbar ...\n";

		if (newData.bboxData.bDecompileModels())
		{
			wcout << L"... Reading decompiled hitbox files ...\n";

			if (newData.bboxData.bReadModelFiles())
			{
				wcout << L"... Reading weapon data from CS:GO items_game.txt ...\n";

				if (newData.sirData.bReadWeapFile(csgoDir))
					bSuccess = true;
				else
					wcout << endl << L"Reading weapon data from CS:GO items_game.txt failed ...\n";
			}
			else
				wcout << endl << L"Reading decompiled hitbox files failed.\n";
		}
		else
		{
			newData.bboxData.bReadModelFiles(true);
			wcout << endl << L"Decompiling hitbox binaries failed.\n";
		}
	}
	else
		wcout << endl << L"Unpacking hitbox files from CS:GO VPK failed.\n";

	return bSuccess;
}

void compAndUpdate(CsgoData &csvData, CsgoData &newData)
{
	wcout << L"... done.";
	//Dereferences and references below allow use of dynamic_cast, which can only be used with pointers and references to classes
	//dynamic_cast ensures the result of the type conversion points to a valid, complete object of the destination pointer type
	csvData.bboxData.compareArchives(newData.bboxData);
	csvData.sirData.compareArchives(newData.sirData);

	if (csvData.bboxData.getNumNonMatches() == 0 && csvData.sirData.getNumNonMatches() == 0)
		wcout << L" No discrepancies detected.\n";
	else
	{
		wcout << endl;
		listNonMatches(csvData.bboxData);
		listNonMatches(csvData.sirData);

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

void updatePrompt(CsgoData &csvData, const CsgoData &newData)
{
	int menuOption = 0;

	do
	{
		wcout << endl << L"Would you like to update the archive file with the new game data?\n";
		wcout << L"Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt(2, L"12"))
		{
		case 1:
			csvData.bboxData = newData.bboxData; //Copy assignment operator in Archive safely copies over data in dynamic arrays
			csvData.sirData = newData.sirData;

			if (csvData.bboxData.bWriteArchiveFile() && csvData.sirData.bWriteArchiveFile())
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

void pickCalcParams(Calc::Params &calcParams, const CsgoData &csvData, const Calc::WhichParam whichParams)
{
	if (whichParams == Calc::WhichParam::ALL || whichParams == Calc::WhichParam::MODEL_INDEX)
		calcParams.modelIndex = pickModelSide(csvData.bboxData);

	if (whichParams == Calc::WhichParam::ALL || whichParams == Calc::WhichParam::WEAPON_INDEX)
		calcParams.weaponIndex = pickWeapon(csvData.sirData);

	if (whichParams == Calc::WhichParam::ALL || whichParams == Calc::WhichParam::DISTANCE)
		calcParams.distance = pickDistance();
}

int pickModelSide(const BboxData &bboxData)
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
			modelIndex = pickModelBase(bboxData, wstring(L"ct"));
			break;
		case 2:
			modelIndex = pickModelBase(bboxData, wstring(L"tm"));
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}

		if (modelIndex == -1)
			menuOption = 0;
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int pickModelBase(const BboxData &bboxData, const wstring modelPrefix)
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

		for (int i = 0; i < bboxData.getNumRows(); ++i)
		{
			if (bboxData.getRowHeader(i).substr(0, menuModels[numBaseModels].length()) != menuModels[numBaseModels]
				&& bboxData.getRowHeader(i).substr(0, 2) == modelPrefix) //List only CT or T models
			{
				size_t lastUnderscore = bboxData.getRowHeader(i).find_last_of(L'_');

				if (bboxData.getRowHeader(i).find_first_of(L'_') != lastUnderscore)
					menuModels[++numBaseModels] = bboxData.getRowHeader(i).substr(0, lastUnderscore);
				else
					menuModels[++numBaseModels] = bboxData.getRowHeader(i);

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
			modelIndex = pickModelVariant(bboxData, menuModels[menuOption]);

			if (modelIndex == -1)
				menuOption = 0;
		}
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int pickModelVariant(const BboxData &bboxData, const wstring baseModel)
{
	int modelIndex;
	int i;
	int menuOption;
	int numVariants;

	do
	{
		wstring validChars;

		wcout << endl << endl;

		for (i = 0; i < bboxData.getNumRows(); ++i) //Find first baseModel variants
		{
			if (bboxData.getRowHeader(i).substr(0, baseModel.length()) == baseModel)
			{
				numVariants = 0;

				while (numVariants < bboxData.getNumRows() - i)
				{
					if (bboxData.getRowHeader(i + numVariants).substr(0, baseModel.length()) != baseModel)
						break; //Exit loop if base model changes

					wcout << ++numVariants << L" - " << bboxData.getRowHeader(i + numVariants - 1) << endl; //Build variant menu
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
	} while (menuOption == 0); //Loops until a valid menu option is input

	modelIndex = -1 + (menuOption == numVariants ? 0 : i + menuOption); //Restarts base model selection if conditional is true

	return modelIndex;
}

wchar_t intDigitToWchar(const int intDigit)
{
	return static_cast<wchar_t>(intDigit + static_cast<int>(L'0'));
}

int pickWeapon(const SirData &sirData)
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

			for (menuNumber = 1; menuNumber <= 8 && i < sirData.getNumRows(); ++menuNumber) //Lists weapons 8 at a time until
			{
				wcout << menuNumber << L" - " << sirData.getRowHeader(i) << endl; //Builds weapon menu
				validChars += intDigitToWchar(menuNumber);
				++i;
			}

			if (i != sirData.getNumRows()) //Option to display next set of weapons if unlisted ones remain
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
				i -= menuNumber; //Resets i to its value before this loop iteration
			}
			else if (menuOption != displayMoreNum)
				weaponIndex = i - menuNumber + menuOption - (displayMoreNum == 0 ? 1 : 0);
		} while (menuOption == 0); //Loops until a valid menu option is input
	} while (weaponIndex == -1);

	return weaponIndex;
}

double pickDistance()
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

		//More than one decimal, more than 2 decimal places, or not from 0.01-300.00 feet inclusive
		if ((distance.find_last_of(L'.') != decimalPos || distance.substr(decimalPos + 1).length() > 2)
			&& decimalPos != distance.npos || stod(distance) < 0.01 || stod(distance) > 300.00)
		{
			distance.clear();
		}
	} while (distance.empty());

	return stod(distance) * 12.0; //Convert to inches
}

void calcIdealFreq(const Calc::Params &calcParams, const CsgoData &csvData)
{
	Calc calculation(calcParams, csvData.sirData);
	const double targetRadius = stod(csvData.bboxData.getDatum(calcParams.modelIndex, csvData.bboxData.getNumColumns() - 1));
	double targetInaccuracy = targetRadius / (0.001 * calcParams.distance);
	double tapInterval = calculation.tapInterval(targetInaccuracy);

	if (tapInterval == 0.0)
		wcout << endl << endl << L"Weapon is not accurate enough to have 100% tapping accuracy with current criteria.";
	else
	{
		wcout << endl << endl << L"Ideal tap-fire interval: " << tapInterval << " seconds";
		double cycleTime = stod(csvData.sirData.getDatum(calcParams.weaponIndex, csvData.sirData.fetchColumnIndex(L"cycletime")));
		wcout << (tapInterval == cycleTime ? " (max firing speed)" : "") << endl << endl << endl;
	}
}

bool bUserMenu(int &menuOption, Calc::Params &calcParams, const CsgoData &csvData)
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
				wcout << L"4 - modify move speed\n";
				wcout << L"5 - modify tapping accuracy\n";
				wcout << L"6 - modify distance\n";
				wcout << L"7 - modify tickrate\n";
				wcout << L"8 - no more modifications; perform the calculation\n";
				wcout << L"Please enter a choice from the preceding menu options: ";
				subMenuOption = takeOnlyOneInt(8, L"12345678");

				if (subMenuOption == 8)
					break;

				if (subMenuOption != 0)
					pickCalcParams(calcParams, csvData, static_cast<Calc::WhichParam>(subMenuOption));
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
	} while (menuOption == 0); //Loops until a valid menu option is input

	return bContinue;
}

} //namespace sircat

int main()
{
	using namespace sircat;

	wchar_t startWchar;

	wcout << endl << L"SirCat will check for updated game data if CS:GO is installed.\n";
	wcout << L"Hit enter to begin... ";
	bTakeOnlyOneWchar(startWchar);
	wcout << endl << endl << L"If CS:GO is found, a helper program called Crowbar will run automatically.\n";
	wcout << L"User input will be disabled while it runs. Press Ctrl+Alt+Del if it hangs up.\n\n";

	Calc::Params calcParams;
	CsgoData *csvData = nullptr;
	int menuOption = 1;

	do
	{
		if (menuOption == 1) //Controlled by switch in bUserMenu for re-running the program
		{
			CsgoData *newData = nullptr;

			if (csvData != nullptr)
				delete csvData;

			csvData = new CsgoData(wstring(L"archiveBboxData.csv"), wstring(L"archiveSirData.csv"));

			if (!csvData->bboxData.getBSuccessUseCsv() || !csvData->sirData.getBSuccessUseCsv())
			{
				delete csvData;
				hitEnterToExit();
			}

			newData = new CsgoData(*csvData);

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
