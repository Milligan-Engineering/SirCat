#pragma once

#include "bbox\BboxArchive.h"	//Using sircat::csgo::bbox::BboxArchive
#include "bbox\BboxFresh.h"		//Using sircat::csgo::bbox::BboxFresh
#include "sir\SirArchive.h"		//Using sircat::csgo::sir::SirArchive
#include "sir\SirFresh.h"		//Using sircat::csgo::sir::SirFresh
#include "GameData.h"			//Using sircat::csgo::GameData
#include <string>				//Using std::wstring

namespace sircat {
namespace csgo {

class DataPair
{
public:
	DataPair(const std::wstring bboxCsvName, const std::wstring sirCsvName) : pBbox(new bbox::BboxArchive(bboxCsvName)),
																			  pSir(new sir::SirArchive(sirCsvName)) {};
	DataPair(const bbox::BboxArchive &bboxArchive, const sir::SirArchive &sirArchive) : pBbox(new bbox::BboxFresh(bboxArchive)),
																						pSir(new sir::SirFresh(sirArchive)) {};
	DataPair(const DataPair &other) = delete;
	virtual ~DataPair();

	DataPair &operator= (const DataPair &otherDataPair);

	bool bCompareDiscrepancies(const DataPair &otherDataPair) const;

	virtual GameData &getBbox() const = 0;
	virtual GameData &getSir() const = 0;
protected:
	GameData *pBbox;
	GameData *pSir;
};

} //namespace csgo
} //namespace sircat
