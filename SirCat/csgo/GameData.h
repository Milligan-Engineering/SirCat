#pragma once

#include <iosfwd>
#include <string>

namespace sircat {
namespace util { class TextFileOps; }
namespace csgo {

class GameData
{
public:
	struct NonMatch
	{
		std::wstring otherRowHeader;
		std::wstring commonColumnHeader;
		std::wstring otherDatum;
		std::wstring datum;
	};

	virtual ~GameData();

	GameData &operator= (const GameData &otherGameData);

	int compareGameData(const GameData &otherGameData, const bool bGetNonMatchSize = false);

	void deleteNonMatches();

	bool bWriteCsvFile(const std::wstring csvName);

	int fetchColumnIndex(const std::wstring columnHeader) const;

	int getNumColumns() const;
	int getNumRows() const;
	std::wstring getColumnHeader(const int j) const;
	std::wstring getRowHeader(const int i) const;
	std::wstring getDatum(const int i, const int j) const;
	int getNumNonMatches() const;
	const NonMatch *const getNonMatches() const;
protected:
	GameData() noexcept;

	GameData(const GameData &otherGameData, const void *const);

	GameData(const GameData &other) = delete;

	int numColumns;
	int numRows;
	sircat::util::TextFileOps *textFileOps;
	std::wstring *columnHeaders;
	std::wstring *rowHeaders;
	std::wstring **data;
private:
	void allocNonMatches(const GameData &otherGameData, int &ret);

	void writeCsvFileRow(const std::wstring newRow[]) const;

	std::wofstream &getOutCsv() const;

	int numNonMatches;
	NonMatch *nonMatches;
	std::wofstream *outCsv;
};

} //namespace csgo
} //namespace sircat
