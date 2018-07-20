#pragma once

#include "GameData.h"				//Using sircat::csgo::GameData
#include "..\util\TextFileOps.h"	//Using sircat::util::TextFileOps
#include <string>					//Using std::wstring

namespace sircat {
namespace csgo {

class Archive : public GameData
{
public:
	virtual ~Archive() = default;
	Archive &operator= (const Archive &other) = delete;

	bool bWriteArchiveFile();

	bool getBSuccessUseCsv() const;
	std::wstring getCsvName() const;
protected:
	Archive(const std::wstring csvName);
private:
	void useCsv(sircat::util::TextFileOps::Params &params);

	bool bSuccessUseCsv;
	std::wstring csvName;
};

} //namespace csgo
} //namespace sircat
