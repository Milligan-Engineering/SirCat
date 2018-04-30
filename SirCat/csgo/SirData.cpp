#include "SirData.h"
#include "..\TextFileOps\TextFileOps.h"
#include <fstream>
#include <string>

using namespace std;

struct SirData::AltMode
{
	const wchar_t *weaponName;
	const wchar_t *altModeName;
};

SirData::SirData()
{
	setAltModes();
}

SirData::SirData(const wstring csvName) : Archive(csvName)
{
	setAltModes();
}

bool SirData::bReadWeapFile(const wstring csgoDir)
{
	//Constants relating to CS:GO game data in items_game.txt
	const int k_num_unparsed_attr = 70;
	const int k_data_len = 10;
	const int k_attr_len = 30;

	bool bSuccess = false;
	wifstream weapFile;

	weapFile.open(csgoDir + wstring(L"\\csgo\\scripts\\items\\items_game.txt"));

	if (!weapFile.fail())
	{
		for (int i = 0; i < numRows; ++i) //Collect weapon data for each weapon
		{
			wchar_t searchResult[1][_MAX_PATH];
			wstring searchTerm = wstring(L"\"") + rowHeaders[i] + wstring(L"_prefab\"");
			wchar_t unparsedData[k_num_unparsed_attr][_MAX_PATH];
			wchar_t parsedWeapData[k_num_unparsed_attr][k_data_len];
			int unparsedAttr;

			textFileOps->parseTextFile(searchTerm, weapFile, searchResult, 1);
			searchTerm = L"\"attributes\""; //Read until attributes are listed for each weapon
			unparsedAttr = textFileOps->parseTextFile(searchTerm, weapFile, unparsedData, _MAX_PATH, L"\t\"\0", 2, L'}');

			for (int j = 0; j < unparsedAttr; ++j) //Enumerate all returned unparsed attributes for each weapon
			{
				bool bNumberInUnparsedData = false;

				for (int k = 0; k < k_attr_len; ++k) //Enumerate characters for each returned unparsed attribute
				{
					if (iswdigit(static_cast<wint_t>(unparsedData[j][k])))
					{
						int l;

						bNumberInUnparsedData = true;

						//Copy up to k_data_len = 10 digit characters in parsedWeaponData
						for (l = 0; l < k_data_len && unparsedData[j][k + l] != L'\0'; ++l)
						{
							parsedWeapData[j][l] = unparsedData[j][k + l];
							unparsedData[j][k + l] = L'\0';
						}

						parsedWeapData[j][l] = L'\0'; //Add terminal null character to character array

						//Change trailing characters to null characters, leaving parsed attribute names in unparsedData
						while (unparsedData[j][k + l] != L'\0')
						{
							unparsedData[j][k + l] = L'\0';
							++l;
						}
					}

					if (bNumberInUnparsedData)
						break; //Stop enumerating characters for unparsed attribute if number digit was found
				}

				for (int m = 0; m < numColumns; ++m) //unparsedData now is parsed attribute names, so compare to stored ones
				{
					if (unparsedData[j] == columnHeaders[m])
						data[i][m] = parsedWeapData[j];
				}
			}
		}

		weapFile.close(); //Close and reopen to start searching from the beginning
		weapFile.open(csgoDir + wstring(L"\\csgo\\scripts\\items\\items_game.txt"));

		if (!weapFile.fail())
		{
			wchar_t defCycletime[1][_MAX_PATH];

			textFileOps->parseTextFile(wstring(L"\"cycletime\""), weapFile, defCycletime, 1, L"\t\"\0", 2);

			for (int i = 0; i < numRows; ++i)
			{
				if (data[i][0] == L"") //Weapons missing cycletime get the default value
					data[i][0] = defCycletime[0];
			}

			bSuccess = true;
		}

		weapFile.close();
	}

	return bSuccess;
}

SirData::AltMode *SirData::getAltModes() const
{
	return altModes;
}

void SirData::setAltModes()
{
	AltMode setAltModes[6];
	setAltModes[0] = { L"weapon_aug", L"scoped" };
	setAltModes[1] = { L"weapon_ssg556", L"scoped" };
	setAltModes[2] = { L"weapon_g3sg1", L"scoped" };
	setAltModes[3] = { L"weapon_scar20", L"scoped" };
	setAltModes[4] = { L"weapon_m4a1_silencer", L"silencer-off" };
	setAltModes[5] = { L"weapon_usp_silencer", L"silencer-off" };

	altModes = setAltModes;
}
