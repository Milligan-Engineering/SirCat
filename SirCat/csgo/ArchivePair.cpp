#include "ArchivePair.h"

namespace sircat {
namespace csgo {

bbox::BboxArchive &ArchivePair::getBboxArchive() const
{
	return bboxArchive;
}

sir::SirArchive &ArchivePair::getSirArchive() const
{
	return sirArchive;
}

} //namespace csgo
} //namespace sircat
