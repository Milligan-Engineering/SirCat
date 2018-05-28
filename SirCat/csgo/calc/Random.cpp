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
using std::uint8_t;
using std::seed_seq;

uint8_t Random::GeneratePseudorandomSeed() const
{
	array<uint32_t, 1> seed;
	seed_seq seq{ static_cast<uint32_t>(high_resolution_clock::now().time_since_epoch().count()) };

	seq.generate(seed.begin(), seed.end());

	return static_cast<uint8_t>(seed[0] & UINT8_MAX);
}

void Random::SetSeed(uint8_t seed)
{
	seed += 1;
	idum = (seed < 0 ? seed : -seed);
	iy = 0;
}

double Random::RandomDouble(double dblMinVal, double dblMaxVal)
{
	if (bAutoSeed)
	{
		SetSeed(GeneratePseudorandomSeed());
		bAutoSeed = false;
	}

	double dbl = k_am * GenerateRandomNumber();

	if (dbl > k_rnmx)
		dbl = k_rnmx;

	return dbl * (dblMaxVal - dblMinVal) + dblMinVal;
}

void Random::setBAutoSeed(const bool b_newAutoSeed)
{
	bAutoSeed = b_newAutoSeed;
}

int Random::GenerateRandomNumber()
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
