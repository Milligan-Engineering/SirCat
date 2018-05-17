#include "SirFresh.h"
#include "..\..\util\TextFileOps.h"
#include <cwctype>
#include <fstream>
#include <string>
#include <stdlib.h>

namespace sircat {
namespace csgo {
namespace sir {

using sircat::util::TextFileOps;
using std::iswdigit;
using std::wifstream;
using std::wstring;

bool SirFresh::bReadWeapFile(const wstring csgoDir)
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

void SirFresh::fetchWeaponSirData(const int i, wifstream &weapFile)
{
	constexpr int k_attr_len = 30; //Constants relating to CS:GO game data in items_game.txt
	constexpr int k_data_len = 10;
	constexpr int k_num_unparsed_attr = 70;

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

} //namespace sir
} //namespace csgo
} //namespace sircat
