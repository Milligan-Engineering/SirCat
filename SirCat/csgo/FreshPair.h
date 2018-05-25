#pragma once

#include "ArchivePair.h"
#include "DataPair.h"
#include "bbox\BboxArchive.h"
#include "bbox\BboxFresh.h"
#include "sir\SirArchive.h"
#include "sir\SirFresh.h"
#include "GameData.h"

namespace sircat {
namespace csgo {

class FreshPair : public DataPair
{
public:
	FreshPair(const ArchivePair &archivePair) : DataPair(archivePair.getBboxArchive(), archivePair.getSirArchive()),
												bboxFresh(dynamic_cast<bbox::BboxFresh &>(*pBbox)),
												sirFresh(dynamic_cast<sir::SirFresh &>(*pSir)) {};

	bbox::BboxFresh &getBboxFresh() const;
	sir::SirFresh &getSirFresh() const;
private:
	bbox::BboxFresh &bboxFresh;
	sir::SirFresh &sirFresh;
};

} //namespace csgo
} //namespace sircat
