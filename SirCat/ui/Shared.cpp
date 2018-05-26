#include "Shared.h"
#include "..\csgo\ArchivePair.h"
#include "..\csgo\FreshPair.h"
#include "..\csgo\DataPair.h"
#include "..\csgo\bbox\BboxArchive.h"
#include "..\csgo\sir\SirArchive.h"
#include "..\csgo\GameData.h"
#include "..\csgo\calc\Calc.h"
#include "..\csgo\FindCsgo.h"
#include <string>

namespace sircat {
namespace ui {

using csgo::ArchivePair;
using csgo::FreshPair;
using csgo::DataPair;
using csgo::bbox::BboxArchive;
using csgo::GameData;
using csgo::calc::Calc;
using csgo::FindCsgo;
using std::stod;
using std::wstring;

int Shared::attemptFind(FindCsgo &findCsgo, wstring &steamDir) const
{
	int successfulSteps = 0;

	if (findCsgo.bFetchSteamDir(steamDir))
	{
		++successfulSteps;

		if (findCsgo.bCheckCsgoInstall() //CSGO found in default Steam library
			|| findCsgo.bSearchSteamLibs()) //CSGO found in alternate Steam library
			++successfulSteps;
	}

	return successfulSteps;
}

double Shared::calcTapInterval(const Calc::Params &calcParams, const ArchivePair &archivePair) const
{
	double tapInterval;
	Calc calculation(calcParams, archivePair.getSirArchive());
	const double k_radius = stod(archivePair.getBboxArchive().getDatum(calcParams.modelIndex,
																	   archivePair.getBboxArchive().getNumColumns() - 1));
	double targetInaccuracy = k_radius / (0.001 * calcParams.distance);

	tapInterval = calculation.tapInterval(targetInaccuracy);

	return tapInterval;
}

bool Shared::bReadGameFilesStep(const wstring csgoDir, FreshPair &freshPair, int step, wstring &retWString) const
{
	bool bSuccess = false;

	retWString.clear();

	switch (step)
	{
	case k_unpack_models:
		if (freshPair.getBboxFresh().bUnpackModels(csgoDir))
		{
			retWString = L"... Decompiling hitbox binaries with Crowbar ...";
			bSuccess = true;
		}
		else
			retWString = L"Unpacking hitbox files from CS:GO VPK failed.";

		break;
	case k_decompile_models:
		if (freshPair.getBboxFresh().bDecompileModels())
		{
			retWString = L"... Reading decompiled hitbox files ...";
			bSuccess = true;
		}
		else
		{
			freshPair.getBboxFresh().bReadModelFiles(true);
			retWString = L"Decompiling hitbox binaries failed.";
		}

		break;
	case k_read_model_files:
		if (freshPair.getBboxFresh().bReadModelFiles())
		{
			retWString = L"... Reading weapon data from CS:GO items_game.txt ...";
			bSuccess = true;
		}
		else
			retWString = L"Reading decompiled hitbox files failed.";

		break;
	case k_read_weap_file:
		if (freshPair.getSirFresh().bReadWeapFile(csgoDir))
			bSuccess = true;
		else
			retWString = L"Reading weapon data from CS:GO items_game.txt failed ...";

		break;
	default:
		break;
	}

	return bSuccess;
}

bool Shared::bUpdateArchive(ArchivePair &archivePair, const FreshPair &freshPair) const
{
	bool bUpdated = false;
	
	dynamic_cast<csgo::DataPair &>(archivePair) = dynamic_cast<const csgo::DataPair &>(freshPair);

	if (archivePair.getBboxArchive().bWriteArchiveFile() && archivePair.getSirArchive().bWriteArchiveFile())
		bUpdated = true;

	return bUpdated;
}

bool Shared::bAddModelBase(const BboxArchive &bboxArchive, const wstring modelPrefix, const int i,
						   int &numBaseModels, wstring menuModels[]) const
{
	bool bAddedModelBase = false;

	if (bboxArchive.getRowHeader(i).substr(0, menuModels[numBaseModels].length()) != menuModels[numBaseModels]
		&& bboxArchive.getRowHeader(i).substr(0, 2) == modelPrefix) //List only CT or T models
	{
		size_t lastUnderscore = bboxArchive.getRowHeader(i).find_last_of(L'_');

		if (bboxArchive.getRowHeader(i).find_first_of(L'_') != lastUnderscore)
			menuModels[++numBaseModels] = bboxArchive.getRowHeader(i).substr(0, lastUnderscore);
		else
			menuModels[++numBaseModels] = bboxArchive.getRowHeader(i);

		bAddedModelBase = true;
	}

	return bAddedModelBase;
}

} //namespace ui
} //namespace sircat
