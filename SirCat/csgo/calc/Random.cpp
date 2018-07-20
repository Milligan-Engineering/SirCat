#include "Random.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <random>

namespace sircat {
namespace csgo {
namespace calc {

using std::array;
using std::chrono::high_resolution_clock;
using std::uint32_t;
using std::seed_seq;

void Random::setSeed(int iSeed)
{
	if (iSeed == 0)
		iSeed = generatePseudorandomSeed();

	idum = (iSeed < 0 ? iSeed : -iSeed);
	iy = 0;
}

int Random::generatePseudorandomSeed() const
{
	array<uint32_t, 1> seed;
	seed_seq seq{ static_cast<uint32_t>(high_resolution_clock::now().time_since_epoch().count()) };

	seq.generate(seed.begin(), seed.end());

	return static_cast<int>((seed[0] & UINT8_MAX) + 1);
}

float Random::randomFloat(float flMinVal, float flMaxVal)
{
	float fl = 1.f * generateRandomNumber() / k_im;

	constexpr float k_rnmx = 1.f - 1.2e-7f;

	if (fl > k_rnmx)
		fl = k_rnmx;

	return fl * (flMaxVal - flMinVal) + flMinVal;
}

int Random::generateRandomNumber()
{
	int j, k;

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
