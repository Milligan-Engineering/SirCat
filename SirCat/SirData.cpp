#include "SirData.h"
#include "TextFileOps.h"
#include <fstream>
#include <string>

string SirData::weapNames[] = { "" };
string SirData::weapAlts[] = { "" };
string SirData::attrNames[] = { "" };

bool SirData::bIsDigit(const char character)
{
	bool bIsDigit = false;
	char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	for (int i = 0; i < 10; ++i)
	{
		if (character == digits[i])
		{
			bIsDigit = true;
			break;
		}
	}

	return bIsDigit;
}

SirData::SirData(const char setCsvName[])
{
	int col = k_num_attr + 2; //Column number that contains the names for usable alt firing modes

	csvName = static_cast<string>(setCsvName);

	if (TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, weapNames, k_num_weap, false, 2) == k_num_weap &&
		TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, weapAlts, k_num_weap, false, 2, ',', col) == k_num_weap &&
		TextFileOps::inst().fetchDelimitedSlice(getInArchive(), csvName, attrNames, k_num_attr, true, 2) == k_num_attr)
	{
		numColumns = k_num_attr;
		bStaticVarsInitialized = true;
	}
}

SirData::SirData()
{
	bStaticVarsInitialized = false;
}

bool SirData::getBStaticVarsInitialized()
{
	return bStaticVarsInitialized;
}

bool SirData::bReadWeapFile(const string csgoDir)
{
	//Constants relating to CS:GO game data in items_game.txt
	const int k_num_unparsed_attr = 70;
	const int k_data_len = 10;
	const int k_attr_len = 30;

	bool bParseSuccess = false;
	ifstream weapFile;

	weapFile.open(csgoDir + static_cast<string>("\\csgo\\scripts\\items\\items_game.txt"));

	if (!weapFile.fail())
	{
		for (int i = 0; i < k_num_weap; ++i) //Collect weapon data for each weapon
		{
			char searchResult[1][TextFileOps::k_max_path];
			string searchTerm = static_cast<string>("\"") + weapNames[i] + static_cast<string>("_prefab\"");
			char unparsedData[k_num_unparsed_attr][TextFileOps::k_max_path];
			char parsedWeapData[k_num_unparsed_attr][k_data_len];
			int unparsedAttr;

			//Read until attributes are listed for each weapon
			TextFileOps::inst().parseTextFile(searchTerm, weapFile, searchResult, 1);
			searchTerm = static_cast<string>("\"attributes\"");
			unparsedAttr = TextFileOps::inst().parseTextFile(searchTerm, weapFile, unparsedData, TextFileOps::k_max_path,
				"\t\"\0", 2, '}');

			for (int j = 0; j < unparsedAttr; ++j) //Enumerate all returned unparsed attributes for each weapon
			{
				bool bNumberInUnparsedData = false;

				for (int k = 0; k < k_attr_len; ++k) //Enumerate characters for each returned unparsed attribute
				{
					if (bIsDigit(unparsedData[j][k])) //Book's isdigit is throwing an exception for me
					{
						int l;

						bNumberInUnparsedData = true;

						//Copy up to k_data_len = 10 digit characters in parsedWeaponData
						for (l = 0; l < k_data_len && unparsedData[j][k + l] != '\0'; ++l)
						{
							parsedWeapData[j][l] = unparsedData[j][k + l];
							unparsedData[j][k + l] = '\0';
						}
						parsedWeapData[j][l] = '\0'; //Add terminal null character to character array

						//Change trailing characters to null characters, leaving parsed attribute names in unparsedData
						while (unparsedData[j][k + l] != '\0')
						{
							unparsedData[j][k + l] = '\0';
							++l;
						}
					}

					if (bNumberInUnparsedData)
						break; //Stop enumerating characters for unparsed attribute if number digit was found
				}

				for (int m = 0; m < k_num_attr; ++m) //unparsedData now is parsed attribute names, so compare to stored ones
				{
					if (unparsedData[j] == attrNames[m])
						sirData[i][m] = static_cast<string>(parsedWeapData[j]);
				}
			}
		}

		weapFile.close(); //Close and reopen to start searching from the beginning
		weapFile.open(csgoDir + static_cast<string>("\\csgo\\scripts\\items\\items_game.txt"));

		if (!weapFile.fail())
		{
			char defCycletime[1][TextFileOps::k_max_path];

			TextFileOps::inst().parseTextFile(static_cast<string>("\"cycletime\""), weapFile, defCycletime, 1, "\t\"\0", 2);

			for (int i = 0; i < k_num_weap; ++i)
			{
				if (sirData[i][0] == "") //Weapons missing cycletime get the default value
					sirData[i][0] = static_cast<string>(defCycletime[0]);
			}

			bParseSuccess = true;
		}

		weapFile.close();
	}

	return bParseSuccess;
}

bool SirData::bCheckArchive(SirData &newSir)
{
	bool bUpdate = false;

	for (int i = 0; i < k_num_weap && !bUpdate; ++i) //&& !bUpdate will terminate the loop after first mismatch
		bUpdate = bCheckArchiveRow(weapNames[i], attrNames, newSir.sirData[i], sirData[i]);

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

		getOutArchive() << ",use alt mode" << endl;

		for (int i = 0; i < k_num_weap; ++i)
		{
			getOutArchive() << weapNames[i];

			writeArchiveFileRow(newSir.sirData[i]);

			getOutArchive() << ',' << weapAlts[i] << endl;
		}

		bWriteSuccess = true;
		getOutArchive().close();
	}

	return bWriteSuccess;
}
