#include "DataPair.h"

#include "GameData.h"	//Using sircat::csgo::GameData

namespace sircat {
namespace csgo {

DataPair::~DataPair()
{
	if (pBbox != nullptr)
		delete pBbox;

	if (pSir != nullptr)
		delete pSir;
}

DataPair &DataPair::operator= (const DataPair &otherDataPair)
{
	if (this != &otherDataPair) //No self-assignment
	{
		dynamic_cast<GameData &>(*pBbox) = dynamic_cast<GameData &>(*otherDataPair.pBbox);
		dynamic_cast<GameData &>(*pSir) = dynamic_cast<GameData &>(*otherDataPair.pSir);
	}

	return *this;
}

bool DataPair::bCompareDiscrepancies(const DataPair &otherDataPair) const
{
	bool bDiscrepancies = false;

	pBbox->compareGameData(*otherDataPair.pBbox);
	pSir->compareGameData(*otherDataPair.pSir);

	if (pBbox->getNumNonMatches() != 0 || pSir->getNumNonMatches() != 0)
		bDiscrepancies = true;

	return bDiscrepancies;
}

} //namespace csgo
} //namespace sircat
