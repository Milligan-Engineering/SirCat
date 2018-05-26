#pragma once

#include "..\Archive.h"
#include <string>

namespace sircat {
namespace csgo {
namespace sir {

class SirArchive : public Archive
{
public:
	struct NumAltModes
	{
		enum { k_n = 6 };
	};

	struct AltMode
	{
		const std::wstring weaponName;
		const std::wstring altModeName;

		AltMode(const wchar_t weaponName[], const wchar_t altModeName[]) : weaponName(std::wstring(weaponName)),
																		   altModeName(std::wstring(altModeName)) {};

		AltMode(const AltMode &otherAltMode) = delete;
	};

	SirArchive(const std::wstring csvName) : Archive(csvName), k_alt_modes{ { L"weapon_aug", L"scoped" },
																			{ L"weapon_ssg556", L"scoped" },
																			{ L"weapon_g3sg1", L"scoped" },
																			{ L"weapon_scar20", L"scoped" },
																			{ L"weapon_m4a1_silencer", L"silencer-off" },
																			{ L"weapon_usp_silencer", L"silencer-off" } } {};

	const AltMode *const getK_alt_modes() const { return k_alt_modes; };
private:
	const AltMode k_alt_modes[NumAltModes::k_n];
};

} //namespace sir
} //namespace csgo
} //namespace sircat
