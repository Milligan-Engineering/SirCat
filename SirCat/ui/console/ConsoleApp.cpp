#include "ConsoleApp.h"

#include "..\..\csgo\ArchivePair.h"
#include "..\..\csgo\FreshPair.h"
#include "..\..\csgo\bbox\BboxArchive.h"
#include "..\..\csgo\sir\SirArchive.h"
#include "..\..\csgo\Archive.h"
#include "..\..\csgo\calc\Calc.h"
#include "..\..\csgo\FindCsgo.h"
#include <cstddef>
#include <cwctype>
#include <iostream>
#include <locale>
#include <string>

namespace sircat {
namespace ui {
namespace console {

using csgo::ArchivePair;
using csgo::FreshPair;
using csgo::bbox::BboxArchive;
using csgo::sir::SirArchive;
using csgo::Archive;
using csgo::calc::Calc;
using csgo::FindCsgo;
using std::size_t;
using std::iswdigit;
using std::endl; using std::fixed; using std::wcin; using std::wcout;
using std::locale;
using std::stof; using std::wstring;

void ConsoleApp::introDialogue() const
{
	wcout.imbue(locale(""));
	wcout << fixed;
	wcout << endl << L"SirCat will check for updated game data if CS:GO is installed.\n";
	wcout << L"Hit enter to begin ... ";

	wchar_t startWchar;

	bTakeOnlyOneWchar(startWchar);
	wcout << endl << endl << L"If CS:GO is found, a helper program called Crowbar will run automatically.\n";
	wcout << L"User input will be disabled while it runs. Press Ctrl+Alt+Del if it hangs up.\n\n";
}

void ConsoleApp::hitEnterToExit() const
{
	wcout << L"Failed to correctly retrieve archived data.\n\n\n";
	wcout << endl << L"Hit enter to exit: ";

	wchar_t exitWchar;

	bTakeOnlyOneWchar(exitWchar);
	wcout << endl;
}

void ConsoleApp::attemptFindCsgo(ArchivePair &archivePair, FreshPair &freshPair) const
{
	bool bFoundCsgo = false;
	FindCsgo findCsgo;
	wstring steamDir;
	int successfulSteps = attemptFind(findCsgo, steamDir);

	if (successfulSteps >= k_found_steam)
	{
		wcout << L"Steam installation found in directory:\n" << steamDir << endl << endl;

		if (successfulSteps == k_found_csgo)
		{
			wcout << L"CS:GO installation found in directory:\n" << findCsgo.getTestDir() << endl << endl;
			wcout << L"Checking fresh CS:GO hitbox and weapon data against file archive data.\n";

			if (bReadGameFiles(findCsgo.getTestDir(), freshPair))
			{
				bFoundCsgo = true;
				compAndUpdate(archivePair, freshPair);
			}
		}
		else
			wcout << L"CS:GO installation not found.\n";
	}
	else
		wcout << L"Steam installation not found.\n";

	if (!bFoundCsgo)
		wcout << L"Using hitbox and weapon data from archive file.\n\n";
}

void ConsoleApp::pickCalcParams(Calc::Params &calcParams, const ArchivePair &archivePair,
								const Calc::WhichParam whichParams) const
{
	if (whichParams == Calc::WhichParam::k_all || whichParams == Calc::WhichParam::k_model_index)
		calcParams.modelIndex = pickModelSide(archivePair.getBbox());

	if (whichParams == Calc::WhichParam::k_all || whichParams == Calc::WhichParam::k_weapon_index)
		calcParams.weaponIndex = pickWeapon(archivePair.getSir());

	if (whichParams == Calc::WhichParam::k_all || whichParams == Calc::WhichParam::k_distance)
		calcParams.distance = pickDistance();
}

void ConsoleApp::calcIdealFreq(Calc::Params &calcParams, const ArchivePair &archivePair) const
{
	wcout << endl << endl << L"Calcuating ...\n";

	float cycleTime = stof(archivePair.getSir().getDatum(calcParams.weaponIndex,
														 archivePair.getSir().fetchColumnIndex(L"cycletime")));
	float maxInterval = 0.f;
	float tapInterval = calcTapInterval(calcParams, archivePair, maxInterval);

	wcout << L" ... done. ~";
	wcout << static_cast<long long>(((tapInterval - cycleTime) / calcParams.tapIncr)
									+ 1.f) * calcParams.simsPerTap * calcParams.maxTaps;
	wcout << L" random-spread shot simulations were performed.\n";

	if (tapInterval >= maxInterval)
		wcout << endl << L"Weapon is not accurate enough for 100% tapping accuracy with current criteria.";
	else
	{
		wcout << endl << L"Ideal tap-fire interval: " << tapInterval << " seconds";
		wcout << (tapInterval == cycleTime ? " (max firing speed)" : "") << endl << endl << endl;
	}
}

bool ConsoleApp::bUserMenu(int &menuOption, Calc::Params &calcParams, const ArchivePair &archivePair) const
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
			pickModifications(calcParams, archivePair);
			++menuOption; //Controls flow in main.cpp
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

bool ConsoleApp::bTakeOnlyOneWchar(wchar_t &character) const
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

bool ConsoleApp::bReadGameFiles(const wstring csgoDir, FreshPair &freshPair) const
{
	bool bSuccess = false;

	wcout << L"... Unpacking hitbox binaries from CS:GO VPK with HLExtract ...\n";

	wstring retWString;

	for (int step = k_start; step < k_finish; ++step)
	{
		bSuccess = bReadGameFilesStep(csgoDir, freshPair, step, retWString);

		if (!bSuccess)
		{
			wcout << endl << retWString << endl;
			break;
		}
		else if (!retWString.empty())
			wcout << retWString << endl;
	}

	return bSuccess;
}

void ConsoleApp::compAndUpdate(ArchivePair &archivePair, const FreshPair &freshPair) const
{
	wcout << L"... done.";

	if (!archivePair.bCompareDiscrepancies(freshPair))
		wcout << L" No discrepancies detected.\n";
	else
	{
		wcout << endl;
		listNonMatches(archivePair.getBbox());
		listNonMatches(archivePair.getSir());

		updatePrompt(archivePair, freshPair);
	}
}

void ConsoleApp::listNonMatches(const Archive &archive) const
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

void ConsoleApp::updatePrompt(ArchivePair &archivePair, const FreshPair &freshPair) const
{
	int menuOption = 0;

	do
	{
		wcout << endl << L"Would you like to update the archive file with the new game data?\n";
		wcout << L"Please enter 1 for yes, or 2 for no: ";

		switch (menuOption = takeOnlyOneInt(2, L"12"))
		{
		case 1:
			if (bUpdateArchive(archivePair, freshPair))
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

int ConsoleApp::takeOnlyOneInt(const int numValidChars, const wchar_t validChars[]) const
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

int ConsoleApp::wcharDigitToInt(const wchar_t wcharDigit) const
{
	return (static_cast<int>(wcharDigit) - static_cast<int>(L'0'));
}

unsigned int ConsoleApp::pickModelSide(const BboxArchive &bboxArchive) const
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
			modelIndex = pickModelBase(bboxArchive, L"ct");
			break;
		case 2:
			modelIndex = pickModelBase(bboxArchive, L"tm");
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}

		if (modelIndex == -1)
			menuOption = 0;
	} while (menuOption == 0); //Loop until a valid menu option is input

	return static_cast<unsigned int>(modelIndex);
}

int ConsoleApp::pickModelBase(const BboxArchive &bboxArchive, const wstring modelPrefix) const
{
	int modelIndex = -1;

	int menuOption;
	int numBaseModels;

	do
	{
		wstring menuModels[9];
		wstring validChars;

		numBaseModels = 0;
		menuModels[0] = L" ";
		wcout << endl << endl;

		for (int i = 0; i < bboxArchive.getNumRows(); ++i)
		{
			if (bAddModelBase(bboxArchive, modelPrefix, i, numBaseModels, menuModels))
			{
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
			modelIndex = pickModelVariant(bboxArchive, menuModels[menuOption]);

			if (modelIndex == -1)
				menuOption = 0;
		}
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int ConsoleApp::pickModelVariant(const BboxArchive &bboxArchive, const wstring baseModel) const
{
	int modelIndex;

	int i;
	int menuOption;
	int numVariants = 0;

	do
	{
		wstring validChars;

		wcout << endl << endl;

		for (i = 0; i < bboxArchive.getNumRows(); ++i) //Find first baseModel variants
		{
			if (bboxArchive.getRowHeader(i).substr(0, baseModel.length()) == baseModel)
			{
				numVariants = 0;

				while (numVariants < bboxArchive.getNumRows() - i)
				{
					if (bboxArchive.getRowHeader(i + numVariants).substr(0, baseModel.length()) != baseModel)
						break; //Exit loop if base model changes

					wcout << ++numVariants << L" - " << bboxArchive.getRowHeader(i + numVariants - 1) << endl; //Variant menu
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

wchar_t ConsoleApp::intDigitToWchar(const int intDigit) const
{
	return static_cast<wchar_t>(intDigit + static_cast<int>(L'0'));
}

unsigned int ConsoleApp::pickWeapon(const SirArchive &sirArchive) const
{
	int weaponIndex = -1;

	int i = 0;
	int menuOption = 0;
	int menuNumber;

	do
	{
		wstring validChars;

		menuNumber = 1;
		wcout << endl << endl;

		do
		{
			wcout << menuNumber << L" - " << sirArchive.getRowHeader(i) << endl; //Builds weapon menu
			validChars += intDigitToWchar(menuNumber);
			++i;
			++menuNumber;
		} while (menuNumber <= 8 && i < sirArchive.getNumRows()); //Lists weapons 8 at a time until

		validChars += intDigitToWchar(menuNumber);

		if (i != sirArchive.getNumRows())
			wcout << menuNumber << L" - display more weapons\n"; //Option to display next set of weapons if unlisted ones remain
		else
			wcout << menuNumber << L" - go back to start of weapon list\n";

		wcout << L"Select a weapon to use, or display more weapons to choose from: ";
		menuOption = takeOnlyOneInt(menuNumber, validChars.c_str());

		if (menuOption == 0)
		{
			wcout << endl << endl << L"That is not a valid menu option.";
			i -= menuNumber - 1; //Resets i to its value before this loop iteration
		}
		else if (menuOption != menuNumber)
			weaponIndex = i - menuNumber + menuOption;
		else if (i == sirArchive.getNumRows())
			i = 0; //Goes back to start of weapon list
	} while (menuOption == 0 || menuOption == menuNumber);

	return static_cast<unsigned int>(weaponIndex);
}

float ConsoleApp::pickDistance() const
{
	wstring distance = L"not empty";

	do
	{
		if (distance.empty())
		{
			wcout << endl << endl << L"That distance is not valid.";
			wcout << endl << L"Requires 0.010000 <= distance <= 300.000000 and 6 or fewer decimal places.";
		}

		wcout << endl << endl << L"Input distance from target in feet (0.010000-300.000000): ";
		getline(wcin, distance);

		for (size_t i = 0u; i < distance.length(); ++i)
		{
			if (iswdigit(static_cast<wint_t>(distance[i])) == 0 && distance[i] != L'.')
			{
				distance.clear(); //Non-number characters in input distance
				break;
			}
		}

		const size_t decimalPos = distance.find_first_of(L'.');

		//Evaluates true when more than one decimal, more than 6 decimal places, or not from 0.010000-300.000000 feet inclusive
		if ((distance.find_last_of(L'.') != decimalPos || distance.substr(decimalPos + 1).length() > 6u)
			&& decimalPos != distance.npos || stof(distance) < 0.010000f || stof(distance) > 300.000000f)
			distance.clear();
	} while (distance.empty());

	return stof(distance) * 12.f; //Converts to inches
}

void ConsoleApp::pickModifications(Calc::Params &calcParams, const ArchivePair &archivePair) const
{
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
		wcout << L"8 - specify calculation precision parameters\n";
		wcout << L"9 - no more modifications; perform the calculation\n";
		wcout << L"Please enter a choice from the preceding menu options: ";
		subMenuOption = takeOnlyOneInt(9, L"123456789");

		if (subMenuOption == 9)
			break;

		if (subMenuOption == 8)
		{
			int subSubMenuOption = 0;

			do
			{
				wcout << endl << endl;
				wcout << L"1 - specify max number of consecutive taps\n";
				wcout << L"2 - specify min tap interval increase\n";
				wcout << L"3 - specify number of random distribution simulations per tap\n";
				wcout << L"4 - go back to modification selection\n";
				wcout << L"Please enter a choice from the preceding menu options: ";
				subSubMenuOption = takeOnlyOneInt(4, L"1234");

				if (subSubMenuOption == 0)
					wcout << endl << endl << L"That is not a valid menu option.";
				else if (subMenuOption == 4)
					subMenuOption = -1;
				else
					subMenuOption += subSubMenuOption;
			} while (subSubMenuOption == 0);
		}

		if (subMenuOption > 0)
			pickCalcParams(calcParams, archivePair, static_cast<Calc::WhichParam>(subMenuOption));
		else if (subMenuOption == 0)
			wcout << endl << endl << L"That is not a valid menu option.";
	} while (true);
}

} //namespace console
} //namespace ui
} //namespace sircat
