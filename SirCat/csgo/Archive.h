#pragma once

#include <iosfwd>
#include <string>

using namespace std;

class TextFileOps;

class Archive
{
public:
	struct NonMatch
	{
		wstring otherRowHeader;
		wstring commonColumnHeader;
		wstring otherDatum;
		wstring datum;
	};

	void temp(const Archive *const otherArchive, bool b1, bool b2, bool b3, bool b4, bool b5);///////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	int compareArchives(const Archive *const otherArchive, const bool bGetNonMatchSize, NonMatch **nonMatches = nullptr) const;
	//Precondition: 
	//Postcondition: 

	bool bWriteArchiveFile(const wstring csvName = wstring());
	//Precondition: 
	//Postcondition: 

	wstring getCsvName() const;
	//Precondition: 
	//Postcondition: 

	bool getBSuccessUseCsv() const;
	//Precondition: 
	//Postcondition: 
protected:
	Archive();
	//Precondition: 
	//Postcondition: 

	Archive(const wstring csvName);
	//Precondition: 
	//Postcondition: 

	~Archive();
	//Precondition: 
	//Postcondition: 

	wifstream &getInArchive() const;
	//Precondition: 
	//Postcondition: 

	wofstream &getOutArchive() const;
	//Precondition: 
	//Postcondition: 

	int numRows;
	int numColumns;
	wstring *rowHeaders;
	wstring *columnHeaders;
	wstring **data;
	wstring csvName;
	TextFileOps *textFileOps;
	wifstream *inArchive;
	wofstream *outArchive;
private:
	void useCsv();
	//Precondition: 
	//Postcondition: 

	void writeArchiveFileRow(const wstring newRow[]);
	//Precondition: 
	//Postcondition: 

	bool bSuccessUseCsv;
};
