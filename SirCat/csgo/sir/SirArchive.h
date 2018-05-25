#pragma once

#include "..\Archive.h"
#include <string>

namespace sircat {
namespace csgo {
namespace sir {

class SirArchive : public Archive
{
public:
	struct AltMode
	{
		const wchar_t *weaponName;
		const wchar_t *altModeName;
	};

	SirArchive() = delete;
	SirArchive(const std::wstring csvName) : Archive(csvName) {};
	SirArchive(const SirArchive &otherSirArchive) = delete;
	~SirArchive() = default;
	SirArchive &operator= (const SirArchive &otherSirArchive) = delete;

	const AltMode *getAltModes() const { return altModes; };
private:
	const AltMode altModes[6] =
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
