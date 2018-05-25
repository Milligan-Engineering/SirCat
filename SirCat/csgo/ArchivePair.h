#pragma once

#include "DataPair.h"
#include "bbox\BboxArchive.h"
#include "sir\SirArchive.h"
#include "GameData.h"
#include <string>

namespace sircat {
namespace csgo {

class ArchivePair : public DataPair
{
public:
	ArchivePair(const std::wstring bboxCsvName,
				const std::wstring sirCsvName) : DataPair(bboxCsvName, sirCsvName),
												 bboxArchive(dynamic_cast<bbox::BboxArchive &>(*pBbox)),
												 sirArchive(dynamic_cast<sir::SirArchive &>(*pSir)) {};

	bbox::BboxArchive &getBboxArchive() const;
	sir::SirArchive &getSirArchive() const;
private:
	bbox::BboxArchive &bboxArchive;
	sir::SirArchive &sirArchive;
};

} //namespace csgo
} //namespace sircat
