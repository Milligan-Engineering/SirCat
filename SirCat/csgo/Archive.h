#pragma once

#include "GameData.h"
#include "..\util\TextFileOps.h"
#include <string>

namespace sircat {
namespace csgo {

class Archive : public GameData
{
public:
	virtual ~Archive() = default;

	bool bWriteArchiveFile();

	bool getBSuccessUseCsv() const;
	std::wstring getCsvName() const;
protected:
	Archive() = delete;
	Archive(const std::wstring csvName);
	Archive(const Archive &otherArchive) = delete;
	Archive &operator= (const Archive &otherArchive) = delete;
private:
	void useCsv(sircat::util::TextFileOps::Params &params);

	bool bSuccessUseCsv;
	std::wstring csvName;
};

} //namespace csgo
} //namespace sircat
