#pragma once

#include "..\GameData.h"	//Using sircat::csgo::GameData
#include "SirArchive.h"		//Using sircat::csgo::sir::SirArchive
#include <iosfwd>			//Using std::wifstream
#include <string>			//Using std::wstring

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

	void fillInMissingValue(const int i, const int j, const std::wstring assignValue);

};

} //namespace sir
} //namespace csgo
} //namespace sircat
