#ifndef STRICT //Enforce strict definitions of Windows data types
	#define STRICT
#endif //STRICT

#ifndef WIN32_LEAN_AND_MEAN //Exclude rarely-used stuff from Windows headers
	#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "SirData.h"
#include "TextFileOps.h"
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

bool SirData::bArchiveObjMade = false;
wstring SirData::weapNames[] = { L"" };
wstring SirData::weapAlts[] = { L"" };
wstring SirData::attrNames[] = { L"" };

bool SirData::bMakeSirObjArchive(const wstring csvName)
{
	bool bSuccess = false;

	if (!bArchiveObjMade)
	{
		wstring *headers[3] = { weapNames, weapAlts, attrNames };
		const int sliceSize[3] = { k_num_weap, k_num_weap, k_num_attr };
		const bool sliceIsRow[3] = { false, false, true };
		const int numSlice[3] = { 1, k_num_attr + 2, 1 };

		Archive::csvName = csvName;
		numColumns = k_num_attr;
		if (bMakeObjArchive(3, headers, sliceSize, sliceIsRow, numSlice))
		{
			bArchiveObjMade = true;
			bSuccess = true;
		}
	}

	return bSuccess;
}

bool SirData::bReadWeapFile(const wstring csgoDir)
{
	//Constants relating to CS:GO game data in items_game.txt
	const int k_num_unparsed_attr = 70;
	const int k_data_len = 10;
	const int k_attr_len = 30;

	bool bParseSuccess = false;
	wifstream weapFile;

	weapFile.open(csgoDir + static_cast<wstring>(L"\\csgo\\scripts\\items\\items_game.txt"));

	if (!weapFile.fail())
	{
		for (int i = 0; i < k_num_weap; ++i) //Collect weapon data for each weapon
		{
			WCHAR searchResult[1][MAX_PATH];
			wstring searchTerm = static_cast<wstring>(L"\"") + weapNames[i] + static_cast<wstring>(L"_prefab\"");
			WCHAR unparsedData[k_num_unparsed_attr][MAX_PATH];
			WCHAR parsedWeapData[k_num_unparsed_attr][k_data_len];
			int unparsedAttr;

			//Read until attributes are listed for each weapon
			TextFileOps::inst().parseTextFile(searchTerm, weapFile, searchResult, 1);
			searchTerm = static_cast<wstring>(L"\"attributes\"");
			unparsedAttr = TextFileOps::inst().parseTextFile(searchTerm, weapFile, unparsedData, MAX_PATH, L"\t\"\0", 2, L'}');

			for (int j = 0; j < unparsedAttr; ++j) //Enumerate all returned unparsed attributes for each weapon
			{
				bool bNumberInUnparsedData = false;

				for (int k = 0; k < k_attr_len; ++k) //Enumerate characters for each returned unparsed attribute
				{
					if (iswdigit(static_cast<wint_t>(unparsedData[j][k]))) //Book's isdigit is throwing an exception for me
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

				for (int m = 0; m < k_num_attr; ++m) //unparsedData now is parsed attribute names, so compare to stored ones
				{
					if (unparsedData[j] == attrNames[m])
						sirData[i][m] = static_cast<wstring>(parsedWeapData[j]);
				}
			}
		}

		weapFile.close(); //Close and reopen to start searching from the beginning
		weapFile.open(csgoDir + static_cast<wstring>(L"\\csgo\\scripts\\items\\items_game.txt"));

		if (!weapFile.fail())
		{
			WCHAR defCycletime[1][MAX_PATH];

			TextFileOps::inst().parseTextFile(static_cast<wstring>(L"\"cycletime\""), weapFile, defCycletime, 1, L"\t\"\0", 2);

			for (int i = 0; i < k_num_weap; ++i)
			{
				if (sirData[i][0] == L"") //Weapons missing cycletime get the default value
					sirData[i][0] = static_cast<wstring>(defCycletime[0]);
			}

			bParseSuccess = true;
		}

		weapFile.close();
	}

	return bParseSuccess;
}

bool SirData::bCheckArchive(SirData &newSir, wstring &badRowName, wstring &badColName, wstring &badNewVal,
	wstring &badArchiveVal)
{
	bool bUpdate = false;
	int j;

	for (int i = 0; i < k_num_weap; ++i)
	{
		if (bUpdate = bCheckArchiveRow(weapNames[i], attrNames, newSir.sirData[i], sirData[i], j)) //Single = is intentional
		{
			badRowName = weapNames[i];
			badColName = attrNames[j];
			badNewVal = newSir.sirData[i][j];
			badArchiveVal = sirData[i][j];
			break; //Terminate the loop after first mismatch
		}
	}

	return bUpdate;
}

void SirData::readArchive()
{
	for (int i = 0; i < k_num_weap; ++i)
		readArchiveRow(sirData[i], i + 2);
}

bool SirData::bWriteArchiveFile(SirData &newSir)
{
	bool bWriteSuccess = false;

	getOutArchive().open(csvName);

	if (!getOutArchive().fail())
	{
		writeArchiveFileRow(attrNames);
		getOutArchive() << L",use alt mode" << endl;

		for (int i = 0; i < k_num_weap; ++i)
		{
			getOutArchive() << weapNames[i];
			writeArchiveFileRow(newSir.sirData[i]);
			getOutArchive() << L',' << weapAlts[i] << endl;
		}

		bWriteSuccess = true;
		getOutArchive().close();
	}

	return bWriteSuccess;
}
