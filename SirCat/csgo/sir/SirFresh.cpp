#include "SirFresh.h"
#include "..\..\util\TextFileOps.h"
#include <cwctype>
#include <fstream>
#include <string>

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
			wstring defCycletime[1];

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
	wstring parsedWeapData[k_num_unparsed_attr];
	wstring searchResult[1];
	wstring searchTerm = wstring(L"\"") + rowHeaders[i] + L"_prefab\"";
	wstring unparsedData[k_num_unparsed_attr];

	textFileOps->parseTextFile(searchTerm, weapFile, searchResult, 1);
	searchTerm = L"\"attributes\""; //Read until attributes are listed for each weapon
	unparsedAttrs = textFileOps->parseTextFile(searchTerm, weapFile, unparsedData, k_num_unparsed_attr, L"\t\"\0", 2, L'}');

	for (int j = 0; j < unparsedAttrs; ++j) //Enumerate all returned unparsed attributes for each weapon
	{
		int kmax = unparsedData[j].length() > k_attr_len ? k_attr_len : unparsedData[j].length();

		for (int k = 0; k < kmax; ++k) //Enumerate characters for each returned unparsed attribute
		{
			if (iswdigit(static_cast<wint_t>(unparsedData[j][k])) != 0) //Check for number digit
			{
				int lmax = unparsedData[j].length() - k > k_data_len ? k + k_data_len : unparsedData[j].length();

				for (int l = k; l < lmax; ++l)
					parsedWeapData[j] += unparsedData[j][l]; //Add remaining data from unparsed attribute

				unparsedData[j].erase(k); //unparsedData[j] now contains a parsed attribute name
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
