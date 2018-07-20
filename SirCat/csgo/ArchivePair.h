#pragma once

#include "DataPair.h"			//Using sircat::csgo::DataPair
#include "bbox\BboxArchive.h"	//Using sircat::csgo::bbox::BboxArchive
#include "sir\SirArchive.h"		//Using sircat::csgo::sir::SirArchive
#include "GameData.h"			//Using sircat::csgo::GameData
#include <string>				//Using std::wstring

namespace sircat {
namespace csgo {

class ArchivePair : public DataPair
{
public:
	class AutoDtor
	{
	public:
		AutoDtor(void *pArchivePair) : pArchivePair(pArchivePair) {};
		~AutoDtor() { static_cast<ArchivePair *>(pArchivePair)->~ArchivePair(); };
	private:
		void *pArchivePair;
	};

	ArchivePair(const std::wstring bboxCsvName,
				const std::wstring sirCsvName) : DataPair(bboxCsvName, sirCsvName),
												 bboxArchive(dynamic_cast<bbox::BboxArchive &>(*pBbox)),
												 sirArchive(dynamic_cast<sir::SirArchive &>(*pSir)) {};

	bbox::BboxArchive &getBbox() const override { return bboxArchive; };
	sir::SirArchive &getSir() const override { return sirArchive; };
private:
	bbox::BboxArchive &bboxArchive;
	sir::SirArchive &sirArchive;
};

} //namespace csgo
} //namespace sircat
