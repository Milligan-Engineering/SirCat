//Program: SirCat (Spread, Inaccuracy & Recoil Calculator for Accurate Training) for CS:GO
//File Name: main.cpp
//Author: Casey Williams
//Email Address: cjwilliams@my.milligan.edu
//Assignment: Project Milestone #07
//Description: Program to calculate the optimal frequency for tap-firing at a capsule-shaped target in Counter-Strike: Global Offensive.
//Last Changed: February 20, 2018

#include <iostream>
#include <string>

bool validateInput(bool bVoltageOrCurrent, double testInput, int dataIndex);
//Add preconditions and postconditions

int main()
{
	using namespace std;

	char menuOption;
	bool bInstantaneousPowers = false;
	bool bAveragePower = false;
	char exitLetter;

	do
	{
		cout << endl << "A: Display instantaneous powers only.\n";
		cout << "B: Display average powers only.\n";
		cout << "C: Display instantaneous and average powers.\n";
		cout << "Please select an option from the menu: ";
		cin >> menuOption;
		switch (menuOption)
		{
		case 'A':
		case 'a':
			bInstantaneousPowers = true;
			break;
		case 'B':
		case 'b':
			bAveragePower = true;
			break;
		case 'C':
		case 'c':
			bInstantaneousPowers = true;
			bAveragePower = true;
			break;
		default:
			cout << endl << "That is not a valid menu option.\n";
		}
	} while (!bInstantaneousPowers && !bAveragePower);

	cout << endl << endl << "Type a letter to exit: ";
	cin >> exitLetter;

	return 0;
}