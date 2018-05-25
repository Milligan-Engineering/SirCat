#pragma once

#include "..\GameData.h"
#include "SirArchive.h"
#include <string>

namespace sircat {
namespace csgo {
namespace sir {

class SirFresh : public GameData
{
public:
	SirFresh() = delete;
	SirFresh(const SirArchive &sirArchive) : GameData(sirArchive, nullptr) {};
	SirFresh(const SirFresh &otherSirFresh) = delete;
	~SirFresh() = default;
	SirFresh &operator= (const SirFresh &otherSirFresh) = delete;

	bool bReadWeapFile(const std::wstring csgoDir);
private:
	void fetchWeaponSirData(const int i, std::wifstream &weapFile);
};

} //namespace sir
} //namespace csgo
} //namespace sircat
