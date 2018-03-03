//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Program to calculate the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 2, 2018

#include <iostream>
#include <cstdlib>

bool getSteamRegistryValue(char steamDir[_MAX_PATH]);
//Precondition: 
//Postcondition: 

int concatArray(char cArray1[], int cArray1Size, char cArray2[], int cArray2Size, char returnArray[]);
//Precondition: 
//Postcondition: 

bool findCSGOInstallation(char directory[_MAX_PATH]);
//Precondition: 
//Postcondition: 

int main()
{
	using namespace std;

	char steamDir[_MAX_PATH];

	if (getSteamRegistryValue(steamDir))
	{
		char defaultFolder[] = "\\steamapps";
		char testDirectory[_MAX_PATH];
		int testDirectorySize = concatArray(steamDir, sizeof(steamDir), defaultFolder, sizeof(defaultFolder), testDirectory);

		//
		for (int i = 0; i < sizeof(steamDir); i++)
		{
			cout << i << ": " << /*static_cast<unsigned int>steamDir[i]*/testDirectory[i] << endl;
		}

		if (findCSGOInstallation(steamDir))
		{
			//readWeaponFile();
		}
	}

	char exitLetter;

	cout << endl << endl << "Type a letter to exit: ";
	cin >> exitLetter;

	return 0;
}

bool getSteamRegistryValue(char steamDir[_MAX_PATH])
{
	//function will search for a registry value that Steam creates during installation
	if (bool bGetRegistryValue = true)
	{
		//steamDir = "C:\\Program Files (x86)\\Steam"; //why doesn't this work?

		return true;
	}

	return false;
}

int concatArray(char cArray1[], int cArray1Size, char cArray2[], int cArray2Size, char returnArray[])
{
	using namespace std;

	int concatPosition = cArray1Size;

	for (int i = 0; i < cArray1Size; i++)
	{
		if (static_cast<unsigned int>(cArray1[i]) == -52);
		{
			concatPosition = i;
			break;
		}
		cout << i << ": " << static_cast<unsigned int>(cArray1[i]);
	}

	/*for (int i = 0; i < cArray1Size; i++)
	{
		if (static_cast<unsigned int>(cArray1[i]) == -52) //dirty way of cutting off array upon the first unused indexed variable
		{
			concatPosition = i;
			break;
		}
		returnArray[i] = cArray1[i];
	}
	for (int i = 0; i < cArray2Size && concatPosition + i < cArray1Size; i++)
	{
		if (static_cast<unsigned int>(cArray2[i]) == -52) //dirty way of cutting off array upon the first unused indexed variable
		{
			concatPosition += i;
			break;
		}
			returnArray[concatPosition + 1] = cArray2[i];
	}*/

	return concatPosition;
}

bool findCSGOInstallation(char directory[_MAX_PATH])
{
	return true;
}