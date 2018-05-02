#pragma once

#include "Archive.h"
#include <string>

using namespace std;

class SirData : public Archive
{
public:
	struct AltMode
	{
		const wchar_t *weaponName;
		const wchar_t *altModeName;
	}; //Need full definition to delcare AltMode array member

	SirData();
	//Precondition: 
	//Postcondition: 

	SirData(const wstring csvName);
	//Precondition: 
	//Postcondition: 

	SirData(const SirData &otherSirData);
	//Precondition: 
	//Postcondition: 

	~SirData() = default;

	bool bReadWeapFile(const wstring csgoDir);
	//Precondition: 
	//Postcondition: 

	const AltMode *getAltModes() const;
	//Precondition: 
	//Postcondition: 
private:
	void setAltModes();
	//Precondition: 
	//Postcondition: 

	AltMode altModes[6];
};
