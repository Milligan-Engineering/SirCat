#pragma once

#include <iosfwd>
#include <string>

namespace sircat {
namespace util { class TextFileOps; }
namespace csgo {

class Archive
{
public:
	struct NonMatch
	{
		std::wstring otherRowHeader;
		std::wstring commonColumnHeader;
		std::wstring otherDatum;
		std::wstring datum;
	}; //Need full definition for references to NonMatch structures outside of the class

	int compareArchives(const Archive &otherArchive, const bool bGetNonMatchSize = false);

	bool bWriteArchiveFile(const std::wstring csvName = std::wstring());

	int fetchColumnIndex(const std::wstring columnHeader) const;

	int getNumColumns() const;
	int getNumRows() const;
	std::wstring getColumnHeader(const int j) const;
	std::wstring getRowHeader(const int i) const;
	std::wstring getDatum(const int i, const int j) const;
	bool getBSuccessUseCsv() const;
	int getNumNonMatches() const;
	const NonMatch *const getNonMatches() const;
	std::wstring getCsvName() const;
protected:
	Archive();
	Archive(const std::wstring csvName);
	Archive(const Archive &otherArchive, const void *const voidParam);
	//Comment?

	Archive(const Archive &otherArchive) = delete;
	~Archive();
	Archive &operator= (const Archive &otherArchive);

	std::wifstream &getInArchive() const;
	std::wofstream &getOutArchive() const;

	int numColumns;
	int numRows;
	sircat::util::TextFileOps *textFileOps;
	std::wstring *columnHeaders;
	std::wstring *rowHeaders;
	std::wstring **data;
	std::wifstream *inArchive;
	std::wofstream *outArchive;
private:
	void allocNonMatches(const Archive &otherArchive, int &ret);

	void useCsv();

	void writeArchiveFileRow(const std::wstring newRow[]) const;

	bool bSuccessUseCsv;
	int numNonMatches;
	NonMatch *nonMatches;
	std::wstring csvName;
};

} //namespace csgo
} //namespace sircat
