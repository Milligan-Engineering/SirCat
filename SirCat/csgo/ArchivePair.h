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
	struct Pair
	{
		bbox::BboxArchive &bbox;
		sir::SirArchive &sir;

		Pair() = delete;
		Pair(GameData *bbox, GameData *sir) : bbox(*dynamic_cast<bbox::BboxArchive *>(bbox)),
											  sir(*dynamic_cast<sir::SirArchive *>(sir)) {};
		Pair(const Pair &otherPair) = delete;
		~Pair() = default;
		Pair &operator= (const Pair &otherPair) = delete;
	};

	ArchivePair() = delete;
	ArchivePair(const std::wstring bboxCsvName, const std::wstring sirCsvName) : DataPair(bboxCsvName, sirCsvName),
																				 pair(bbox, sir) {};
	ArchivePair(const ArchivePair &ArchivePair) = delete;
	~ArchivePair();

	ArchivePair &operator= (const ArchivePair &otherArchivePair) = delete;

	const Pair &getPair() const;
private:
	Pair pair;
};

} //namespace csgo
} //namespace sircat
