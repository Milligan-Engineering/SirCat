#pragma once

#include "..\Archive.h"
#include <string>

namespace sircat {
namespace csgo {
namespace bbox {

class BboxArchive : public Archive
{
public:
	BboxArchive(const std::wstring csvName) : Archive(csvName) {};

};

} //namespace bbox
} //namespace csgo
} //namespace sircat
