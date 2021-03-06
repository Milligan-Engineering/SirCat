#pragma once

#include "..\csgo\calc\Calc.h"	//Using sircat::csgo::calc::Calc::Params
#include <string>				//Using std::wstring

namespace sircat {
namespace csgo {

class ArchivePair;
class FreshPair;
class FindCsgo;

namespace bbox { class BboxArchive; }

} //namespace csgo

namespace ui {

class Shared
{
protected:
	enum { k_start = 1, k_found_steam = 1, k_found_csgo, k_unpack_models = 1, k_decompile_models, k_read_model_files,
		   k_read_weap_file, k_finish };

	Shared() = default;
	~Shared() = default;

	int attemptFind(csgo::FindCsgo &findCsgo, std::wstring &steamDir) const;

	float calcTapInterval(csgo::calc::Calc::Params &calcParams, const csgo::ArchivePair &archivePair, float &maxInterval) const;

	bool bReadGameFilesStep(const std::wstring csgoDir, csgo::FreshPair &freshPair, int step, std::wstring &retWString) const;

	bool bUpdateArchive(csgo::ArchivePair &archivePair, const csgo::FreshPair &freshPair) const;

	bool bAddModelBase(const csgo::bbox::BboxArchive &bboxArchive, const std::wstring modelPrefix, const int i,
					   int &numBaseModels, std::wstring menuModels[]) const;

};

} //namespace ui
} //namespace sircat
