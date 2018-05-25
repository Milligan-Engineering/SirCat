#include "FreshPair.h"

namespace sircat {
namespace csgo {

bbox::BboxFresh &FreshPair::getBboxFresh() const
{
	return bboxFresh;
}

sir::SirFresh &FreshPair::getSirFresh() const
{
	return sirFresh;
}

} //namespace csgo
} //namespace sircat
