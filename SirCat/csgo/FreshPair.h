#pragma once

#include "ArchivePair.h"		//Using sircat::csgo::ArchivePair
#include "DataPair.h"			//Using sircat::csgo::DataPair
#include "bbox\BboxArchive.h"	//Using sircat::csgo::bbox::BboxArchive
#include "bbox\BboxFresh.h"		//Using sircat::csgo::bbox::BboxFresh
#include "sir\SirArchive.h"		//Using sircat::csgo::sir::SirArchive
#include "sir\SirFresh.h"		//Using sircat::csgo::sir::SirFresh
#include "GameData.h"			//Using sircat::csgo::GameData

namespace sircat {
namespace csgo {

class FreshPair : public DataPair
{
public:
	FreshPair(const ArchivePair &archivePair) : DataPair(archivePair.getBbox(), archivePair.getSir()),
												bboxFresh(dynamic_cast<bbox::BboxFresh &>(*pBbox)),
												sirFresh(dynamic_cast<sir::SirFresh &>(*pSir)) {};

	bbox::BboxFresh &getBbox() const override { return bboxFresh; };
	sir::SirFresh &getSir() const override { return sirFresh; };
private:
	bbox::BboxFresh &bboxFresh;
	sir::SirFresh &sirFresh;
};

} //namespace csgo
} //namespace sircat
