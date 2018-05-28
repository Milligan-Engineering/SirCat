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
	Archive &operator= (const Archive &other) = delete;

	bool bWriteArchiveFile();

	bool getBSuccessUseCsv() const;
	const std::wstring &getK_csv_name() const;
protected:
	Archive(const std::wstring csvName);

private:
	void useCsv(sircat::util::TextFileOps::Params &params);

	bool bSuccessUseCsv;
	const std::wstring k_csv_name;
};

} //namespace csgo
} //namespace sircat
