#include "SirData.h"
#include "..\TextFileOps\TextFileOps.h"
#include <cwctype>
#include <fstream>
#include <string>

using namespace std;

SirData::SirData()
{
	setAltModes();
}

SirData::SirData(const wstring csvName) : Archive(csvName)
{
	setAltModes();
}

SirData::SirData(const SirData &otherSirData) : Archive(otherSirData, nullptr)
{
	setAltModes();
}

bool SirData::bReadWeapFile(const wstring csgoDir)
{
	bool bSuccess = false;
	wifstream weapFile;

	weapFile.open(csgoDir + L"\\csgo\\scripts\\items\\items_game.txt");

	if (!weapFile.fail())
	{
		for (int i = 0; i < numRows; ++i) //Collect weapon data for each weapon
			fetchWeaponSirData(i, weapFile);

		weapFile.close(); //Close and reopen to start searching from the beginning
		weapFile.open(csgoDir + L"\\csgo\\scripts\\items\\items_game.txt");

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

const SirData::AltMode *SirData::getAltModes() const
{
	return altModes;
}

void SirData::setAltModes()
{
	altModes[0] = { L"weapon_aug", L"scoped" };
	altModes[1] = { L"weapon_ssg556", L"scoped" };
	altModes[2] = { L"weapon_g3sg1", L"scoped" };
	altModes[3] = { L"weapon_scar20", L"scoped" };
	altModes[4] = { L"weapon_m4a1_silencer", L"silencer-off" };
	altModes[5] = { L"weapon_usp_silencer", L"silencer-off" };
}

void SirData::fetchWeaponSirData(const int i, wifstream &weapFile)
{
	const int k_attr_len = 30; //Constants relating to CS:GO game data in items_game.txt
	const int k_data_len = 10;
	const int k_num_unparsed_attr = 70;

	int unparsedAttrs;
	wchar_t parsedWeapData[k_num_unparsed_attr][k_data_len];
	wchar_t searchResult[1][_MAX_PATH];
	wchar_t unparsedData[k_num_unparsed_attr][_MAX_PATH];
	wstring searchTerm = wstring(L"\"") + rowHeaders[i] + L"_prefab\"";

	textFileOps->parseTextFile(searchTerm, weapFile, searchResult, 1);
	searchTerm = L"\"attributes\""; //Read until attributes are listed for each weapon
	unparsedAttrs = textFileOps->parseTextFile(searchTerm, weapFile, unparsedData, _MAX_PATH, L"\t\"\0", 2, L'}');

	for (int j = 0; j < unparsedAttrs; ++j) //Enumerate all returned unparsed attributes for each weapon
	{
		for (int k = 0; k < k_attr_len; ++k) //Enumerate characters for each returned unparsed attribute
		{
			if (iswdigit(static_cast<wint_t>(unparsedData[j][k])) != 0) //Check for number digit in unparsed attribute characters
			{
				int l;

				for (l = 0; l < k_data_len; ++l) //Add relevant remaining data from unpared attribute to parsed weapon data
				{
					if (unparsedData[j][k + l] == L'\0')
						break;

					parsedWeapData[j][l] = unparsedData[j][k + l];
					unparsedData[j][k + l] = L'\0';
				}

				parsedWeapData[j][l] = L'\0'; //Add terminal null character to character array

				while (unparsedData[j][k + l] != L'\0')
				{
					unparsedData[j][k + l] = L'\0'; //Change trailing chars to null chars, leaving parsed attribute names
					++l;
				}

				break; //Stop enumerating characters for unparsed attribute since number digit was found
			}
		}

		for (int m = 0; m < numColumns; ++m) //unparsedData now is parsed attribute names, so compare to stored ones
		{
			if (unparsedData[j] == columnHeaders[m])
				data[i][m] = parsedWeapData[j];
		}
	}
}
