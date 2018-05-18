#include "FreshPair.h"

namespace sircat {
namespace csgo {

FreshPair::~FreshPair()
{
	if (bbox != nullptr)
		delete bbox;

	if (sir != nullptr)
		delete sir;
}

const FreshPair::Pair &FreshPair::getPair() const
{
	return pair;
}

} //namespace csgo
} //namespace sircat
