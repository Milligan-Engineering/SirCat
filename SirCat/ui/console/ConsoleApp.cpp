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
using std::endl;
using std::wcin;
using std::wcout;
using std::stod;
using std::wstring;

void ConsoleApp::introDialogue() const
{
	wchar_t startWchar;
	wcout << endl << L"SirCat will check for updated game data if CS:GO is installed.\n";
	wcout << L"Hit enter to begin... ";
	bTakeOnlyOneWchar(startWchar);
	wcout << endl << endl << L"If CS:GO is found, a helper program called Crowbar will run automatically.\n";
	wcout << L"User input will be disabled while it runs. Press Ctrl+Alt+Del if it hangs up.\n\n";
}

void ConsoleApp::hitEnterToExit() const
{
	wchar_t exitWchar;

	wcout << L"Failed to correctly retrieve archived data.\n\n\n";
	wcout << endl << L"Hit enter to exit: ";
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
		calcParams.modelIndex = pickModelSide(archivePair.getBboxArchive());

	if (whichParams == Calc::WhichParam::k_all || whichParams == Calc::WhichParam::k_weapon_index)
		calcParams.weaponIndex = pickWeapon(archivePair.getSirArchive());

	if (whichParams == Calc::WhichParam::k_all || whichParams == Calc::WhichParam::k_distance)
		calcParams.distance = pickDistance();
}

void ConsoleApp::calcIdealFreq(const Calc::Params &calcParams, const ArchivePair &archivePair) const
{
	double tapInterval = calcTapInterval(calcParams, archivePair);

	if (tapInterval == 0.0)
		wcout << endl << endl << L"Weapon is not accurate enough for 100% tapping accuracy with current criteria.";
	else
	{
		double cycleT = stod(archivePair.getSirArchive().getDatum(calcParams.weaponIndex,
																  archivePair.getSirArchive().fetchColumnIndex(L"cycletime")));

		wcout << endl << endl << L"Ideal tap-fire interval: " << tapInterval << " seconds";
		wcout << (tapInterval == cycleT ? " (max firing speed)" : "") << endl << endl << endl;
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
					pickCalcParams(calcParams, archivePair, static_cast<Calc::WhichParam>(subMenuOption));
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
		listNonMatches(archivePair.getBboxArchive());
		listNonMatches(archivePair.getSirArchive());

		updatePrompt(archivePair, freshPair);
	}
}

void ConsoleApp::listNonMatches(const Archive &archive) const
{
	if (archive.getNumNonMatches() > 0)
	{
		wcout << endl << L"Data non-match detected in " << archive.getK_csv_name() << ":\n";

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

int ConsoleApp::pickModelSide(const BboxArchive &bboxArchive) const
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
			modelIndex = pickModelBase(bboxArchive, wstring(L"ct"));
			break;
		case 2:
			modelIndex = pickModelBase(bboxArchive, wstring(L"tm"));
			break;
		default:
			wcout << endl << endl << L"That is not a valid menu option.";
		}

		if (modelIndex == -1)
			menuOption = 0;
	} while (menuOption == 0); //Loop until a valid menu option is input

	return modelIndex;
}

int ConsoleApp::pickModelBase(const BboxArchive &bboxArchive, const wstring modelPrefix) const
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

int ConsoleApp::pickWeapon(const SirArchive &sirArchive) const
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

	return weaponIndex;
}

double ConsoleApp::pickDistance() const
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

} //namespace console
} //namespace ui
} //namespace sircat
