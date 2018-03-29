#include "FindCsgo.h"
#include "TextFileOps.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

FindCsgo &FindCsgo::inst()
{
	static FindCsgo inst;
	return inst;
}

bool FindCsgo::bFetchSteamDir()
{
	bool bFoundRegValue = false;

	//******Windows functions RegOpenKeyEx, RegQueryValueEx, and RegCloseKey search for the Steam directory if it exists******
	bFoundRegValue = true; //Set to true until Windows functions are added

	if (bFoundRegValue == true)
	{
		char steamDirTemp[] = "C:\\Program Files (x86)\\Steam"; //Replace with RegQueryValueEx output
		char blank[] = "";
		char steamappsFolder[] = "\\steamapps";

		TextFileOps::inst().concatCharArrays(steamDirTemp, blank, testDir, TextFileOps::k_max_path);

		cout << "Steam installation found in directory:\n" << testDir << endl << endl;

		TextFileOps::inst().concatCharArrays(testDir, steamappsFolder, testDir, TextFileOps::k_max_path);
	}

	return bFoundRegValue;
}

bool FindCsgo::bCheckCsgoInstall()
{
	bool bFoundCsgoInstall = false;
	ifstream manifest;

	manifest.open(static_cast<string>(testDir) + static_cast<string>("\\appmanifest_730.acf"));

	if (!manifest.fail())
	{
		char searchResult[1][TextFileOps::k_max_path];

		//Verify CS:GO installation directory listed in manifest file contents
		if (static_cast<bool>(TextFileOps::inst().parseTextFile(static_cast<string>("\"installdir\""), manifest, searchResult, 1,
			"\t\"\0", 2)))
		{
			bFoundCsgoInstall = true;
			char installSubDir[] = "\\common\\";

			TextFileOps::inst().concatCharArrays(testDir, installSubDir, testDir, TextFileOps::k_max_path);
			TextFileOps::inst().concatCharArrays(testDir, searchResult[0], testDir, TextFileOps::k_max_path);
		}

		manifest.close();
	}

	return bFoundCsgoInstall;
}

bool FindCsgo::bSearchSteamLibs()
{
	bool bFoundCsgoInstall = false;
	ifstream libFile;

	libFile.open(static_cast<string>(testDir) + static_cast<string>("\\libraryfolders.vdf"));

	if (!libFile.fail())
	{
		for (int i = 1; i < 10; i++)
		{
			char searchTerm[] = { '\"', intDigitToChar(i), '\"', '\0' };
			char searchResult[1][TextFileOps::k_max_path];

			if (static_cast<bool>(TextFileOps::inst().parseTextFile(static_cast<string>(searchTerm), libFile, searchResult, 1,
				"\t\"\0", 2)))
			{
				char steamappsFolder[] = "\\steamapps";

				TextFileOps::inst().concatCharArrays(searchResult[0], steamappsFolder, testDir, TextFileOps::k_max_path);

				if (bCheckCsgoInstall())
				{
					bFoundCsgoInstall = true;
					break;
				}
			}
			else
				break;
		}

		libFile.close();
	}

	return bFoundCsgoInstall;
}

char FindCsgo::intDigitToChar(const int intDigit)
{
	return (static_cast<char>(intDigit + static_cast<int>('0')));
}

string FindCsgo::getTestDir()
{
	return static_cast<string>(testDir);
}
