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
	}; //Need full definition for references to NonMatch structures outside of the class

	int compareArchives(const Archive &otherArchive, const bool bGetNonMatchSize = false);
	//Precondition: 
	//Postcondition: 

	bool bWriteArchiveFile(const wstring csvName = wstring());
	//Precondition: 
	//Postcondition: 

	int getNumColumns() const;
	//Precondition: 
	//Postcondition: 

	int getNumRows() const;
	//Precondition: 
	//Postcondition: 

	wstring getColumnHeader(const int j) const;
	//Precondition: 
	//Postcondition: 

	wstring getRowHeader(const int i) const;
	//Precondition: 
	//Postcondition: 

	wstring getDatum(const int i, const int j) const;
	//Precondition: 
	//Postcondition: 

	bool getBSuccessUseCsv() const;
	//Precondition: 
	//Postcondition:

	int getNumNonMatches() const;
	//Precondition: 
	//Postcondition:

	const NonMatch *const getNonMatches() const;
	//Precondition: 
	//Postcondition:

	wstring getCsvName() const;
	//Precondition: 
	//Postcondition: 
protected:
	Archive();
	//Precondition: 
	//Postcondition: 

	Archive(const wstring csvName);
	//Precondition: 
	//Postcondition: 

	Archive(const Archive &otherArchive, const void *const voidParam);
	//Precondition: 
	//Postcondition: 

	Archive(const Archive &otherArchive) = delete; //Disallow copy constructor

	~Archive();
	//Precondition: 
	//Postcondition:

	Archive &operator= (const Archive &otherArchive);
	//Precondition: 
	//Postcondition: 

	wifstream &getInArchive() const;
	//Precondition: 
	//Postcondition: 

	wofstream &getOutArchive() const;
	//Precondition: 
	//Postcondition: 

	int numColumns;
	int numRows;
	TextFileOps *textFileOps;
	wstring *columnHeaders;
	wstring *rowHeaders;
	wstring **data;
	wifstream *inArchive;
	wofstream *outArchive;
private:
	void allocNonMatches(const Archive &otherArchive, int &ret);
	//Precondition: 
	//Postcondition: 

	void useCsv();
	//Precondition: 
	//Postcondition: 

	void writeArchiveFileRow(const wstring newRow[]) const;
	//Precondition: 
	//Postcondition: 

	bool bSuccessUseCsv;
	int numNonMatches;
	NonMatch *nonMatches;
	wstring csvName;
};
