#define _USE_MATH_DEFINES

#include "Calc.h"
#include "Random.h"
#include "..\sir\SirArchive.h"
#include <cmath>
#include <string>

namespace sircat {
namespace csgo {
namespace calc {

using sir::SirArchive;
using std::log10;
using std::pow;
using std::round;
using std::stod;
using std::wstring;

Calc::Calc(const Params &params, const SirArchive &sirArchive) : k_tickrate(params.b64Tick ? 64.0 : 128.0), k_params(params),
																 stats{ 0.0 }
{
	wstring stance = wstring();
	wstring statNames[k_num_stats] = { L"cycletime", L"primary clip size", L"max player speed", L"recovery time ",
											L"recovery time  final", L"spread",	L"inaccuracy ", L"inaccuracy fire",
											L"inaccuracy move", L"recoil magnitude", L"recoil magnitude variance",
											L"recoil angle variance" };

	if (params.bCrouch)
		stance = L"crouch";
	else
		stance = L"stand";

	statNames[3] += stance;
	statNames[4].insert(14, stance);
	statNames[6] += stance;

	if (params.bUseAlt)
	{
		for (int i = 5; i < k_num_stats; ++i)
			statNames[i] += L" alt";
	}

	for (int i = k_first_stat; i < k_num_stats; ++i)
		stats[i] = stod(sirArchive.getDatum(params.weaponIndex, sirArchive.fetchColumnIndex(statNames[i])));
}

double Calc::tapInterval(const double targetInaccuracy) const
{
	double tapInterval = stats[k_cycletime];
	//deagle for now, make it change for according to each weapon////////////////////////////////////////////////////////////////
	double maxInterval = log10(1.090000 / 122.330001) * 0.811200 / log10(0.1); //////////////////////////////////////////////////
	double maxInaccuracy;

	do
	{
		maxInaccuracy = 0.0;

		double inaccuracy = stats[k_spread] + stats[k_inaccuracy_stance];
		double totalDecayTime = 0;

		for (int taps = 1; taps < static_cast<int>(stats[k_primary_clip_size]); ++taps)
		{
			double newInaccuracy = calcNewInaccuracy(inaccuracy, tapInterval, totalDecayTime);

			if (newInaccuracy > maxInaccuracy)
				maxInaccuracy = newInaccuracy;

			inaccuracy = newInaccuracy;
		}

		if (maxInaccuracy > targetInaccuracy)
			tapInterval += 0.000001;
	} while (maxInaccuracy > targetInaccuracy && tapInterval < maxInterval);

	if (tapInterval >= maxInterval)
		tapInterval = 0.0; //Indicates to the caller that the weapon is not accurate enough to meet the current criteria

	return tapInterval;
}

double Calc::calcNewInaccuracy(const double inaccuracy, const double tapInterval, double &totalDecayTime) const
{
	double oldTotalDecayTime = totalDecayTime;

	totalDecayTime += tapInterval;

	double tickTapInterval = roundTimeToTick(totalDecayTime) - roundTimeToTick(oldTotalDecayTime);

	return inaccuracy * pow(0.1, tickTapInterval / stats[k_recovery_time]) + stats[k_inaccuracy_fire];
}

double Calc::roundTimeToTick(const double time) const
{
	return round(time * k_tickrate) / k_tickrate;
}

bool Calc::bHitPercentInDistribution() const
{
	bool bGreaterOrEqual = false;

	return bGreaterOrEqual;
}

} //namespace calc
} //namespace csgo
} //namespace sircat
