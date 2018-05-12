#pragma once

#include "..\Archive.h"
#include <string>

namespace sircat {
namespace csgo {
namespace sir {

class SirData : public Archive
{
public:
	struct AltMode
	{
		const wchar_t *weaponName;
		const wchar_t *altModeName;
	};

	SirData() = default;
	SirData(const std::wstring csvName) : Archive(csvName) {};
	SirData(const SirData &otherSirData) : Archive(otherSirData, nullptr) {};
	~SirData() = default;

	bool bReadWeapFile(const std::wstring csgoDir);

	const AltMode *getAltModes() const;
private:
	void fetchWeaponSirData(const int i, std::wifstream &weapFile);

	AltMode altModes[6] =
	{
		{ L"weapon_aug", L"scoped" },
		{ L"weapon_ssg556", L"scoped" },
		{ L"weapon_g3sg1", L"scoped" },
		{ L"weapon_scar20", L"scoped" },
		{ L"weapon_m4a1_silencer", L"silencer-off" },
		{ L"weapon_usp_silencer", L"silencer-off" }
	};
};

} //namespace sir
} //namespace csgo
} //namespace sircat
