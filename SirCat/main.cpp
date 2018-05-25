//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: May 18, 2018

#include "ui\console\ConsoleApp.h"
#include "csgo\ArchivePair.h"
#include "csgo\FreshPair.h"
#include "csgo\bbox\BboxArchive.h"
#include "csgo\sir\SirArchive.h"
#include "csgo\Calc.h"
#include <string>

int main()
{
	using namespace sircat;
	using ui::console::ConsoleApp;
	using csgo::ArchivePair;
	using csgo::FreshPair;
	using csgo::Calc;
	using std::wstring;

	ConsoleApp consoleApp;

	consoleApp.introDialogue();

	Calc::Params calcParams;
	ArchivePair *archivePair = nullptr;
	int menuOption = 1;

	do
	{
		if (menuOption == 1)
		{
			if (archivePair != nullptr)
				delete archivePair;

			archivePair = new ArchivePair(wstring(L"archiveBboxData.csv"), wstring(L"archiveSirData.csv"));

			if (!archivePair->getBboxArchive().getBSuccessUseCsv() || !archivePair->getSirArchive().getBSuccessUseCsv())
			{
				delete archivePair;
				consoleApp.hitEnterToExit();
			}

			FreshPair freshPair(*archivePair);
			consoleApp.attemptFindCsgo(*archivePair, freshPair);
			consoleApp.pickCalcParams(calcParams, *archivePair);
		}

		consoleApp.calcIdealFreq(calcParams, *archivePair);
	} while (consoleApp.bUserMenu(menuOption, calcParams, *archivePair));

	delete archivePair;

	return 0;
}
