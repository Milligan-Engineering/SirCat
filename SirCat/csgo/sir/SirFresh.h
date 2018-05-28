#pragma once

#include "..\GameData.h"
#include "SirArchive.h"
#include <iosfwd>
#include <string>

namespace sircat {
namespace csgo {
namespace sir {

class SirFresh : public GameData
{
public:
	SirFresh(const SirArchive &sirArchive) : GameData(sirArchive, nullptr) {};
	SirFresh &operator= (const SirFresh &other) = delete;

	bool bReadWeapFile(const std::wstring csgoDir);

private:
	void fetchWeaponSirData(const int i, std::wifstream &weapFile);

};

} //namespace sir
} //namespace csgo
} //namespace sircat
