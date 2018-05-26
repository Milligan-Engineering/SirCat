#include "Random.h"
#include <chrono>
#include <cstdlib>

namespace sircat {
namespace csgo {
namespace calc {

using std::chrono::high_resolution_clock;
using std::rand;
using std::srand;

Random::Random() noexcept : k_am(1.0 / k_im), k_rnmx(1.0 - 1.2e-7), iv{}, iy(0)
{
	srand(static_cast<unsigned int>(high_resolution_clock::now().time_since_epoch().count()));
	idum = -(rand() % 256); //Sets a pseduorandom seed value
}

double Random::RandomDouble(double dblMinVal, double dblMaxVal)
{
	double dbl = k_am * GenerateRandomNumber();

	if (dbl > k_rnmx)
		dbl = k_rnmx;

	return (dbl * (dblMaxVal - dblMinVal)) + dblMinVal;
}

int Random::GenerateRandomNumber()
{
	int j;
	int k;

	if (idum <= 0 || iy == 0)
	{
		if (idum > -1)
			idum = 1;
		else
			idum *= -1;

		for (j = k_ntab + 7; j >= 0; --j)
		{
			k = idum / k_iq;
			idum = k_ia * (idum - k * k_iq) - k_ir * k;

			if (idum < 0)
				idum += k_im;

			if (j < k_ntab)
				iv[j] = idum;
		}

		iy = iv[0];
	}

	k = idum / k_iq;
	idum = k_ia * (idum - k * k_iq) - k_ir * k;

	if (idum < 0)
		idum += k_im;

	j = iy / k_ndiv;
	iy = iv[j];
	iv[j] = idum;

	return iy;
}

} //namespace calc
} //namespace csgo
} //namespace sircat
