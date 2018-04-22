#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class SirData : public Archive
{
public:
	struct AltMode;

	SirData();
	//Precondition: 
	//Postcondition:

	SirData(const wstring csvName);
	//Precondition: 
	//Postcondition: 

	~SirData();
	//Precondition: 
	//Postcondition: 

	bool bReadWeapFile(const wstring csgoDir);
	//Precondition: 
	//Postcondition: 

	AltMode *getAltModes() const;
	//Precondition: 
	//Postcondition: 
private:
	void setAltModes();
	//Precondition: 
	//Postcondition: 

	AltMode *altModes;
};
