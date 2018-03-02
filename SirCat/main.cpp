//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Program to calculate the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: March 2, 2018

#include <iostream>

bool getSteamRegistryValue(char steamDir[_MAX_PATH]);
//Precondition: 
//Postcondition: 

void concatArray(char cArray1[], int cArray1Size, char cArray2[], int cArray2Size, char returnArray[]);
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
		char testDirectory[_MAX_PATH];
		char defaultFolder[] = "\\steamapps";
		concatArray(steamDir, sizeof(steamDir), defaultFolder, sizeof(defaultFolder), testDirectory);

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
		steamDir = "C:\\Program Files (x86)\\Steam"; //why doesn't this work?

		return true;
	}

	return false;
}

void concatArray(char cArray1[], int cArray1Size, char cArray2[], int cArray2Size, char returnArray[])
{

}

bool findCSGOInstallation(char directory[_MAX_PATH])
{

}