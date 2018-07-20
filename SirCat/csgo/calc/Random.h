#pragma once

#include <array>	//Using std::array

namespace sircat {
namespace csgo {
namespace calc {

class Random
{
public:
	void setSeed(int iSeed = 0);

	int generatePseudorandomSeed() const;

	float randomFloat(float flMinVal = 0.f, float flMaxVal = 1.f);
private:
	enum { k_ntab = 32, k_ir = 2836, k_ia = 16807, k_iq = 127773, k_im = 2147483647, k_ndiv = (1 + (k_im - 1) / k_ntab) };

	int generateRandomNumber();

	int idum;
	int iy;
	std::array<int, k_ntab> iv;
};

} //namespace calc
} //namespace csgo
} //namespace sircat
