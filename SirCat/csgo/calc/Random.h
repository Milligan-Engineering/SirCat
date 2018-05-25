#pragma once

namespace sircat {
namespace csgo {
namespace calc {

class Random
{
public:
	Random();
	float RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f);
private:
	enum { NTAB = 32, IR = 2836, IA = 16807, IQ = 127773, IM = 2147483647 };

	int m_idum;
	int m_iy;
	int m_iv[NTAB];

	int GenerateRandomNumber();
};

} //namespace calc
} //namespace csgo
} //namespace sircat
