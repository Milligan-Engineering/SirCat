//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Program to calculate the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 8, 2018

#include <iostream>
#include <cstdlib>

bool getSteamDir(char steamDir[]);
//Precondition: 
//Postcondition: 

int concatCharArrays(char cArray1[], char cArray2[], char returnArray[], int sizeConcatArray);
//Precondition: 
//Postcondition: 

bool findCSGOInstallation(char directory[_MAX_PATH]);
//Precondition: 
//Postcondition: 

int main()
{
	using namespace std;

	char steamDir[_MAX_PATH];

	if (getSteamDir(steamDir))
	{
		char defaultFolder[] = "\\steamapps";
		char defaultDir[_MAX_PATH];

		concatCharArrays(steamDir, defaultFolder, defaultDir, sizeof(defaultDir));

		//for (int i = 0; defaultDir[i] != '\0'; i++)
		//{
			//cout << i << ": " << (defaultDir[i]) << endl;
		//}

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

bool getSteamDir(char steamDir[])
{
	bool bGetRegistryValue = true;

	//function will search for a registry value that Steam creates during installation
	if (bGetRegistryValue = true)
	{
		char tempSteamDir[] = "C:\\Program Files (x86)\\Steam";

		for (int i = 0; i < sizeof(tempSteamDir); i++)
		{
			steamDir[i] = tempSteamDir[i];
		}

		return true;
	}

	return false;
}

int concatCharArrays(char cArray1[], char cArray2[], char concatArray[], int sizeConcatArray)
{
	using namespace std;

	int concatIndex = 0;

	for (int i = 0; cArray1[i] != '\0' && concatIndex < sizeConcatArray; i++)
	{
		concatArray[concatIndex] = cArray1[i];
		++concatIndex;
	}

	for (int i = 0; cArray2[i] != '\0' && concatIndex < sizeConcatArray; i++)
	{
		concatArray[concatIndex] = cArray2[i];
		++concatIndex;
	}

	if (concatIndex < sizeConcatArray)
	{
		concatArray[concatIndex] = '\0';
		++concatIndex;
	}

	return concatIndex;
}

bool findCSGOInstallation(char directory[_MAX_PATH])
{
	return true;
}