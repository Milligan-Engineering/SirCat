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
	virtual ~DataPair() = default;
protected:
	DataPair() = delete;
	DataPair(const std::wstring bboxCsvName, const std::wstring sirCsvName) : bbox(new bbox::BboxArchive(bboxCsvName)),
																			  sir(new sir::SirArchive(sirCsvName)) {};

	DataPair(const bbox::BboxArchive &bboxArchive, const sir::SirArchive &sirArchive) : bbox(new bbox::BboxFresh(bboxArchive)),
																						sir(new sir::SirFresh(sirArchive)) {};

	DataPair(const DataPair &otherDataPair) = delete;
	DataPair &operator= (const DataPair &otherDataPair) = delete;

	GameData *bbox;
	GameData *sir;
};

} //namespace csgo
} //namespace sircat
