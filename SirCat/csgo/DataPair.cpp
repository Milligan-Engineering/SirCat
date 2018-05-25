#include "DataPair.h"
#include "bbox\BboxArchive.h"
#include "bbox\BboxFresh.h"
#include "sir\SirArchive.h"
#include "sir\SirFresh.h"
#include "GameData.h"

namespace sircat {
namespace csgo {

DataPair::~DataPair()
{
	if (pBbox != nullptr)
		delete pBbox;

	if (pSir != nullptr)
		delete pSir;
}

} //namespace csgo
} //namespace sircat
