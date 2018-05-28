#pragma once

#include "bbox\BboxArchive.h"
#include "bbox\BboxFresh.h"
#include "sir\SirArchive.h"
#include "sir\SirFresh.h"
#include "GameData.h"
#include <string>

namespace sircat {
namespace csgo {

class DataPair
{
public:
	DataPair(const DataPair &other) = delete;
	virtual ~DataPair();

	DataPair &operator= (const DataPair &otherDataPair);

	bool bCompareDiscrepancies(const DataPair &otherDataPair) const;

protected:
	DataPair(const std::wstring bboxCsvName, const std::wstring sirCsvName) : pBbox(new bbox::BboxArchive(bboxCsvName)),
																			  pSir(new sir::SirArchive(sirCsvName)) {};
	DataPair(const bbox::BboxArchive &bboxArchive, const sir::SirArchive &sirArchive) : pBbox(new bbox::BboxFresh(bboxArchive)),
																						pSir(new sir::SirFresh(sirArchive)) {};

	GameData *pBbox;
	GameData *pSir;
};

} //namespace csgo
} //namespace sircat
