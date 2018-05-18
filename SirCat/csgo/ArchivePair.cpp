#include "ArchivePair.h"

namespace sircat {
namespace csgo {

ArchivePair::~ArchivePair()
{
	if (bbox != nullptr)
		delete bbox;

	if (sir != nullptr)
		delete sir;
}

const ArchivePair::Pair &ArchivePair::getPair() const
{
	return pair;
}

} //namespace csgo
} //namespace sircat
