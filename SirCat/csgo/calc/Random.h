#pragma once

#include <cstdint>

namespace sircat {
namespace csgo {
namespace calc {

class Random
{
public:
	Random() = default;
	Random(const bool bAutoSeed) : bAutoSeed(bAutoSeed) {};

	std::uint8_t GeneratePseudorandomSeed() const;

	void SetSeed(std::uint8_t seed);

	double RandomDouble(double dblMinVal = 0.0, double dblMaxVal = 1.0);

	void setBAutoSeed(const bool b_newAutoSeed);
private:
	enum { k_ntab = 32, k_ir = 2836, k_ia = 16807, k_iq = 127773, k_im = 2147483647, k_ndiv = (1 + (k_im - 1) / k_ntab) };

	int GenerateRandomNumber();

	bool bAutoSeed = false;
	double k_am = 1.0 / k_im;
	double k_rnmx = 1.0 - 1.2e-7;
	int idum = 0;
	int iv[k_ntab] = { 0 };
	int iy = 0;
};

} //namespace calc
} //namespace csgo
} //namespace sircat
