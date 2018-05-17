#pragma once

#include "..\Archive.h"
#include <string>

namespace sircat {
namespace csgo {
namespace bbox {

class BboxArchive : public Archive
{
public:
	BboxArchive() = delete;
	BboxArchive(const std::wstring csvName) : Archive(csvName) {};
	~BboxArchive() = default;
};

} //namespace bbox
} //namespace csgo
} //namespace sircat
