#include "Calc.h"
#include "sir\SirData.h"
#include <cmath>
#include <iostream> ///////////////////////////////////////////////////////////////////
#include <string>

namespace sircat {
namespace csgo {

using sir::SirData;
using std::log10;
using std::pow;
using std::round;
using std::stod;
using std::wstring;

Calc::Calc(const Params &params, const SirData &sirData) : tickrate(params.b64Tick ? 64.0 : 128.0)
{
	wstring stance = wstring();
	wstring statNames[Stats::NUM_STATS] = { L"cycletime", L"primary clip size", L"max player speed", L"recovery time ",
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
		for (int i = 5; i < Stats::NUM_STATS; ++i)
			statNames[i] += L" alt";
	}

	for (Stats::I i = Stats::CYCLETIME; i < Stats::NUM_STATS; i = static_cast<Stats::I>(i + 1))
		stats[i] = stod(sirData.getDatum(params.weaponIndex, sirData.fetchColumnIndex(statNames[i])));
}

double Calc::tapInterval(const double targetInaccuracy) const
{
	double tapInterval = stats[Stats::CYCLETIME];
	double maxInterval = log10(1.090000 / 122.330001) * 0.811200 / log10(0.1); //deagle for now, make it change for according to each weapon///////////////////////////////////////////////////
	double newInaccuracy;

	do
	{
		double inaccuracy = stats[Stats::SPREAD] + stats[Stats::INACCURACY_STANCE];
		std::wcout << inaccuracy << std::endl; //////////////////////////////////////////////////////////
		double totalDecayTime = 0;
		
		for (int taps = 1; taps < Stats::PRIMARY_CLIP_SIZE; ++taps);
		{
			newInaccuracy = calcNewInaccuracy(inaccuracy, tapInterval, totalDecayTime);
			std::wcout << newInaccuracy << std::endl; //////////////////////////////////////////////////////////

			if (newInaccuracy > inaccuracy) //newInaccuracy will usually dip below inaccuracy due to rounding to the nearest tick
				break; //Inaccuracy has converged

			inaccuracy = newInaccuracy;
		}

		if (newInaccuracy > targetInaccuracy)
			tapInterval += 1.0 / tickrate;
	} while (newInaccuracy > targetInaccuracy && tapInterval < maxInterval);

	if (tapInterval >= maxInterval)
		tapInterval = 0.0; //Indicates to the caller that the weapon is not accurate enough to meet the current criteria

	return tapInterval;
}

double Calc::calcNewInaccuracy(const double inaccuracy, const double tapInterval, double &totalDecayTime) const
{
	totalDecayTime = round((tapInterval + totalDecayTime) * tickrate) / tickrate; //Calculates inaccuracy during the nearest tick
	return inaccuracy * pow(0.1, tapInterval / stats[Stats::RECOVERY_TIME]) + stats[Stats::INACCURACY_FIRE];
}

} //namespace csgo
} //namespace sircat
