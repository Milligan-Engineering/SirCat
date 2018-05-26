#pragma once

namespace sircat {
namespace csgo {
namespace calc {

class Random
{
public:
	Random() noexcept;

	Random(const Random &otherRandom) = delete;

	double RandomDouble(double dblMinVal = 0.0, double dblMaxVal = 1.0);

private:
	enum { k_ntab = 32, k_ir = 2836, k_ia = 16807, k_iq = 127773, k_im = 2147483647, k_ndiv = (1 + (k_im - 1) / k_ntab) };

	int GenerateRandomNumber();

	const double k_am;
	const double k_rnmx;
	int idum;
	int iv[k_ntab];
	int iy;
};

} //namespace calc
} //namespace csgo
} //namespace sircat
