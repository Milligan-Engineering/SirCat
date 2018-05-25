#include "Random.h"
#include <chrono>
#include <cstdlib>

namespace sircat {
namespace csgo {
namespace calc {

using std::chrono::high_resolution_clock;
using std::rand;
using std::srand;

Random::Random() : m_iy(0)
{
	srand(static_cast<unsigned int>(high_resolution_clock::now().time_since_epoch().count()));
	m_idum = -(rand() % 256); //Sets a pseduorandom seed value
}

float Random::RandomFloat(float flMinVal, float flMaxVal)
{
	float fl = static_cast<float>((1.0 / IM) * static_cast<float>(GenerateRandomNumber()));

	if (fl > 1.0 - 1.2e-7)
		fl = static_cast<float>(1.0 - 1.2e-7);

	return (fl * (flMaxVal - flMinVal)) + flMinVal;
}

int Random::GenerateRandomNumber()
{
	int j;
	int k;

	if (m_idum <= 0 || m_iy == 0)
	{
		if (m_idum > -1)
			m_idum = 1;
		else
			m_idum *= -1;

		for (j = NTAB + 7; j >= 0; --j)
		{
			k = m_idum / IQ;
			m_idum = IA * (m_idum - k * IQ) - IR * k;

			if (m_idum < 0)
				m_idum += IM;

			if (j < NTAB)
				m_iv[j] = m_idum;
		}

		m_iy = m_iv[0];
	}

	k = m_idum / IQ;
	m_idum = IA * (m_idum - k * IQ) - IR * k;

	if (m_idum < 0)
		m_idum += IM;

	j = m_iy / (1 + (IM - 1) / NTAB);
	m_iy = m_iv[j];
	m_iv[j] = m_idum;

	return m_iy;
}

} //namespace calc
} //namespace csgo
} //namespace sircat
