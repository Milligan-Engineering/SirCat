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

	int getNumRows() const;
	//Precondition: 
	//Postcondition: 

	int getNumColumns() const;
	//Precondition: 
	//Postcondition: 

	wstring getRowHeader(const int i) const;
	//Precondition: 
	//Postcondition: 

	wstring getColumnHeader(const int j) const;
	//Precondition: 
	//Postcondition: 

	wstring getDatum(const int i, const int j) const;
	//Precondition: 
	//Postcondition: 

	wstring getCsvName() const;
	//Precondition: 
	//Postcondition: 

	NonMatch *getNonMatches() const;
	//Precondition: 
	//Postcondition: 

	int getNumNonMatches() const;
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

	Archive(const Archive &otherArchive, void *voidParam);
	//Precondition: 
	//Postcondition: 

	Archive(const Archive &otherArchive) = delete; //Don't allow normal copy constructor usage

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
	void allocNonMatches(const Archive &otherArchive, int &ret);
	//Precondition: 
	//Postcondition: 

	void useCsv();
	//Precondition: 
	//Postcondition: 

	void writeArchiveFileRow(const wstring newRow[]);
	//Precondition: 
	//Postcondition: 

	bool bSuccessUseCsv;
	int numNonMatches;
	NonMatch *nonMatches;
};
