#pragma once

#include "..\Shared.h"				//Using sircat::ui::Shared
#include "..\..\csgo\calc\Calc.h"	//Using sircat::csgo::calc::Calc::Params, sircat::csgo::calc::Calc::WhichParam
#include <string>					//Using std::wstring

namespace sircat {
namespace csgo {

class ArchivePair;
class FreshPair;
class Archive;

namespace bbox { class BboxArchive; }
namespace sir { class SirArchive; }

} //namespace csgo

namespace ui {
namespace console {

class ConsoleApp : public Shared
{
public:
	void introDialogue() const;

	void hitEnterToExit() const;
	//Precondition: The console is ready to display a message then receive user input
	//Postcondition: After the user hits enter, the program will exit

	void attemptFindCsgo(csgo::ArchivePair &archivePair, csgo::FreshPair &freshPair) const;
	//Postcondition: Attempts to locate an installation of CS:GO and if found, freshPair is populated with game data
		//archivePair and freshPair are compared and option is given to update the CSV file
		//Returns true if CS:GO is found and the game data is successfully parsed into freshPair, and false otherwise

	void pickCalcParams(csgo::calc::Calc::Params &calcParams, const csgo::ArchivePair &archivePair,
						const csgo::calc::Calc::WhichParam whichParams = csgo::calc::Calc::WhichParam::k_all) const;

	void calcIdealFreq(csgo::calc::Calc::Params &calcParams, const csgo::ArchivePair &archivePair) const;

	bool bUserMenu(int &menuOption, csgo::calc::Calc::Params &calcParams, const csgo::ArchivePair &archivePair) const;
	//Precondition: menuOption and calcParams are modifiable and archivePair is populated with game data
		//The console is ready to display a message then receive user input
	//Postcondition: menuOption is updated according to user input in response to a program menu
		//calcParams is updated according to user inputs if the option is chosen by the user
		//Returns true if the program should continue or false if the program should exit
private:
	bool bTakeOnlyOneWchar(wchar_t &character) const;
	//Precondition: character is modifiable and the console is ready to receive user input
	//Postcondition: Sets character to first character input from the user
	//Returns true if the user input only one character before hitting enter, and false otherwise

	bool bReadGameFiles(const std::wstring csgoDir, csgo::FreshPair &freshPair) const;
	//Precondition: csgoDir is a directory containing a valid install of CS:GO
	//Postcondition: Invokes programs to unpack, decompile, and read game data into freshPair, giving feedback along the way
	//Returns true if the game data is successfully parsed into freshPair, and false otherwise with accompanying error message

	void compAndUpdate(csgo::ArchivePair &archivePair, const csgo::FreshPair &freshPair) const;
	//Precondition: archivePair and freshPair are modifiable and populated with game data
	//Postcondition: archivePair and freshPair are compared, the results are displayed with option to update archive CSV files

	void listNonMatches(const csgo::Archive &archive) const;
	//Postcondition: The non-matching data array from archivePair is parsed and non-matches are displayed

	void updatePrompt(csgo::ArchivePair &archivePair, const csgo::FreshPair &freshPair) const;
	//Precondition: archivePair and freshPair are modifiable and populated with game data
	//The console is ready to display a message then receive user input
	//Postcondition: Option given to update the CSV file with freshPair game data

	int takeOnlyOneInt(const int numValidChars, const wchar_t validChars[]) const;
	//Precondition: numValidChars is the size of validChars and the console is ready to receive user input
	//Postcondition: If user input only one character before hitting enter, returns that character as an integer and 0 otherwise

	int wcharDigitToInt(const wchar_t wcharDigit) const;
	//Precondition: wcharDigit is a digit (0-9)
	//Postcondition: Converts wcharDigit to an integer and returns it

	unsigned int pickModelSide(const csgo::bbox::BboxArchive &bboxArchive) const;
	//Precondition: bboxArchive is populated with game data and the console is ready to display a message then receive user input
	//Postcondition: Loops until model side, base, and variant are selected and returns index of that model

	int pickModelBase(const csgo::bbox::BboxArchive &bboxArchive, const std::wstring modelPrefix) const;
	//Precondition: bboxArchive is populated with game data and the console is ready to display a message then receive user input
	//Postcondition: Loops until model base and variant are selected and returns the index of that model

	int pickModelVariant(const csgo::bbox::BboxArchive &bboxArchive, const std::wstring baseModel) const;
	//Precondition: bboxArchive is populated with game data and the console is ready to display a message then receive user input
	//Postcondition: Loops until model variant is selected and returns the index of that model

	wchar_t intDigitToWchar(const int intDigit) const;
	//Precondition: intDigit is a digit (0-9)
	//Postcondition: Converts intDigit to a character and returns it

	unsigned int pickWeapon(const csgo::sir::SirArchive &sirArchive) const;
	//Precondition: sirArchive is populated with game data and the console is ready to display a message then receive user input
	//Postcondition: To continue, user must pick a weapon, whose index will be returned

	float pickDistance() const;

	void pickModifications(csgo::calc::Calc::Params &calcParams, const csgo::ArchivePair &archivePair) const;

};

} //namespace console
} //namespace ui
} //namespace sircat
