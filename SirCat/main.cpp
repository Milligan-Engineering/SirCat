//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Description: Calculates the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: July 19, 2018

#include "ui\console\ConsoleApp.h"
#include "csgo\ArchivePair.h"
#include "csgo\FreshPair.h"
#include "csgo\bbox\BboxArchive.h"
#include "csgo\sir\SirArchive.h"
#include "csgo\calc\Calc.h"

int main()
{
	using namespace sircat;

	using ui::console::ConsoleApp;
	using csgo::ArchivePair;
	using csgo::FreshPair;
	//Using csgo::bbox::BboxArchive
	//Using csgo::sir::SirArchive
	using csgo::calc::Calc;

	enum { k_start_over = 1, k_skip_destructor };

	ConsoleApp consoleApp;

	consoleApp.introDialogue();

	//Stack-allocates memory for an ArchivePair object and creates automatic storage duration object to call its destructor
	alignas(ArchivePair) unsigned char archivePairAlloc[sizeof(ArchivePair)];
	ArchivePair::AutoDtor autoDtor(static_cast<void *>(archivePairAlloc));
	ArchivePair &archivePair = *static_cast<ArchivePair *>(static_cast<void *>(archivePairAlloc));

	Calc::Params calcParams{ 0u, 0u, false, 0.f, 100.f, 0.f, false, false, 0u, 0.001f, 10000u }; //Sets default Calc::Params
	int menuOption = k_skip_destructor; //Skips archivePair destructor in the first iteration since it is yet to be constructed

	do
	{
		if (menuOption == k_start_over)
			archivePair.~ArchivePair(); //Prepares to make new ArchivePair object on the stack with placement new expression

		if (menuOption <= k_skip_destructor)
		{
			new(archivePairAlloc) ArchivePair(L"archiveBboxData.csv", L"archiveSirData.csv");

			if (!archivePair.getBbox().getBSuccessUseCsv() || !archivePair.getSir().getBSuccessUseCsv())
			{
				consoleApp.hitEnterToExit();
				break;
			}

			FreshPair freshPair(archivePair);

			consoleApp.attemptFindCsgo(archivePair, freshPair);
			consoleApp.pickCalcParams(calcParams, archivePair);
		}

		consoleApp.calcIdealFreq(calcParams, archivePair);
	} while (consoleApp.bUserMenu(menuOption, calcParams, archivePair));

	return 0;
}
