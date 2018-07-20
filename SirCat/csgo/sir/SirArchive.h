#pragma once

#include "..\Archive.h"	//Using sircat::csgo::Archive
#include <array>		//Using std::array
#include <cstddef>		//Using std::size_t
#include <string>		//Using std::wstring

namespace sircat {
namespace csgo {
namespace sir {

class SirArchive : public Archive
{
public:
	struct AltMode
	{
		enum { k_total = 6 };

		const wchar_t *weaponName;
		const wchar_t *altModeName;
	};

	SirArchive(const std::wstring csvName) : Archive(csvName), altModes{ { { L"weapon_aug", L"scoped" },
																		   { L"weapon_ssg556", L"scoped" },
																		   { L"weapon_g3sg1", L"scoped" },
																		   { L"weapon_scar20", L"scoped" },
																		   { L"weapon_m4a1_silencer", L"silencer-off" },
																		   { L"weapon_usp_silencer", L"silencer-off" } } } {};

	AltMode getAltMode(std::size_t i) const { return altModes.at(i); };
private:
	const std::array<AltMode, AltMode::k_total> altModes;
};

} //namespace sir
} //namespace csgo
} //namespace sircat
