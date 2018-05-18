#pragma once

#include "DataPair.h"
#include "ArchivePair.h"
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
	struct Pair
	{
		bbox::BboxFresh &bbox;
		sir::SirFresh &sir;

		Pair() = delete;
		Pair(GameData *bbox, GameData *sir) : bbox(*dynamic_cast<bbox::BboxFresh *>(bbox)),
											  sir(*dynamic_cast<sir::SirFresh *>(sir)) {};
		Pair(const Pair &otherPair) = delete;
		~Pair() = default;
		Pair &operator= (const Pair &otherPair) = delete;
	};

	FreshPair() = delete;
	FreshPair(const ArchivePair::Pair &archivePair) : DataPair(archivePair.bbox, archivePair.sir), pair(bbox, sir) {};
	FreshPair(const FreshPair &otherFreshPair) = delete;
	~FreshPair();

	FreshPair &operator= (const FreshPair &otherFreshPair) = delete;

	const Pair &getPair() const;
private:
	Pair pair;
};

} //namespace csgo
} //namespace sircat
