#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES 1 //M_PI
#endif

#include "Calc.h"

#include "Random.h"
#include "..\ArchivePair.h"
#include "..\bbox\BboxArchive.h"
#include "..\sir\SirArchive.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>

namespace sircat {
namespace csgo {
namespace calc {

//Using Random
//Using ArchivePair
using bbox::BboxArchive;
using sir::SirArchive;
using std::max; using std::min;
//Using std::array
using std::ceilf; using std::cosf; using std::expf; using std::fabsf; using std::floorf; using std::powf; using std::sinf;
				  using std::sqrtf; using std::tanf;
using std::size_t;
using std::stof; using std::wstring;

float Calc::tapInterval(float &maxInterval) const
{
	float tapInterval = sirStats[k_cycletime];

	const bool bIsFullAuto = sirStats[k_is_full_auto] == 1.f;
	const RecoilTable recoilTable = generateRecoilTable(bIsFullAuto);
	const RecoveryTimes recoveryTimes = generateRecoveryTimes(bIsFullAuto);
	float totalTapHitPercent = 0.f;
	Random rand;

	rand.setSeed();
	maxInterval = findMaxInterval(recoilTable, recoveryTimes);

	do
	{
		float inaccuracy = baseInaccuracy;
		float recoilIndex = 0.f;
		float sumDecayTime = 0.f;
		Recoil recoil{};
		Recoil velocity{};
		size_t ticks = 0u;

		recoil = calcRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
		totalTapHitPercent = tapHitPercent(inaccuracy, rand, recoil);

		for (unsigned int taps = 1u; taps < params.maxTaps; ++taps)
		{
			ticks = ticksSinceLastTap(tapInterval, sumDecayTime);
			recoil = calcRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
			inaccuracy = calcInaccuracy(inaccuracy, recoveryTimes[ceilToInt(recoilIndex)], ticks / tickrate);
			totalTapHitPercent += tapHitPercent(inaccuracy, rand, recoil);
		}

		totalTapHitPercent = totalTapHitPercent / params.maxTaps; //Calculates average hit percentage for taps

		if (totalTapHitPercent < params.hitPercent)
			tapInterval += params.tapIncr;
	} while (totalTapHitPercent < params.hitPercent && tapInterval < maxInterval);

	return tapInterval;
}

Calc::Params Calc::getParams() const
{
	return params;
}

void Calc::setParams(Params &inParams, const ArchivePair &archivePair)
{
	fillSirStats(inParams.bUseAlt, inParams.weaponIndex, archivePair.getSir(), inParams.bCrouch ? L"crouch" : L"stand");

	params = inParams;
	params.maxTaps = params.maxTaps == 0u ? static_cast<unsigned int>(sirStats[k_primary_clip_size]) : params.maxTaps;

	const BboxArchive &bbox = archivePair.getBbox();
	const float bbx = stof(bbox.getDatum(params.modelIndex, k_bbmaxx)) - stof(bbox.getDatum(params.modelIndex, k_bbminx));
	const float bby = stof(bbox.getDatum(params.modelIndex, k_bbmaxy)) - stof(bbox.getDatum(params.modelIndex, k_bbminy));
	const float bbz = stof(bbox.getDatum(params.modelIndex, k_bbmaxz)) - stof(bbox.getDatum(params.modelIndex, k_bbminz));

	//Calculates the bbox cylinder half-height from the distance between its two endpoints in three-dimensional Cartesion space
	bboxCylHalfHeight = sqrtf(powf(bbx, 2.f) + powf(bby, 2.f) + powf(bbz, 2.f)) / 2.f;
	bboxRadius = stof(bbox.getDatum(params.modelIndex, k_radius));

	const float moveModifier = powf(min(1.f, max(0.f, params.moveSpeed / sirStats[k_max_player_speed] - 0.34f)
										/ (0.95f - 0.34f)), 0.25f);

	baseInaccuracy = sirStats[k_spread] + sirStats[k_inaccuracy_stance] + moveModifier * sirStats[k_inaccuracy_move];

	tickrate = params.b64Tick ? 64.f : 128.f;
}

Calc::RecoilTable Calc::generateRecoilTable(const bool bIsFullAuto) const
{
	RecoilTable table;

	Random rand;

	rand.setSeed(static_cast<int>(sirStats[k_recoil_seed]));

	for (size_t i = 0u; i < 64u; ++i)
	{
		table[i][k_angle] = rand.randomFloat(-1.f, 1.f) * sirStats[k_recoil_angle_variance];
		table[i][k_magnitude] = sirStats[k_recoil_magnitude] + rand.randomFloat(-1.f, 1.f)
								* sirStats[k_recoil_magnitude_variance];

		if (bIsFullAuto)
		{
			if (i != 0)
			{
				const size_t last = i - 1u;

				table[i][k_angle] = table[last][k_angle] + (table[i][k_angle] - table[last][k_angle]) * 0.55f;
				table[i][k_magnitude] = table[last][k_magnitude] + (table[i][k_magnitude] - table[last][k_magnitude]) * 0.55f;
			}

			if (i < 4)
				table[i][k_magnitude] *= i / 4.f * 0.25f + 0.75f;
		}
	}

	for (size_t i = 64u; i < 150u; ++i)
		table[i] = table[i - 64u];

	return table;
}

Calc::RecoveryTimes Calc::generateRecoveryTimes(const bool bIsFullAuto) const
{
	RecoveryTimes times;

	times.fill(sirStats[k_recovery_time_final]);

	if (sirStats[k_recovery_time] != sirStats[k_recovery_time_final])
	{
		float transitionShots = 3.f;
		size_t i = 0; //Holds the value for shot indexes to set to initial recovery time

		times[i] = sirStats[k_recovery_time];

		if (bIsFullAuto)
		{
			times[++i] = times[++i] = times[i];

			if (sirStats[k_recovery_time] > sirStats[k_recovery_time_final]) //Evaluates to true if weapon is Negev
				times[++i] = times[++i] = times[++i] = times[++i] = times[++i] = times[++i] = times[++i] = times[i];
		}
		else
			transitionShots += 2.f;

		for (int j = 1; j < static_cast<int>(transitionShots); ++j)
			times[++i] = sirStats[k_recovery_time] + j / transitionShots * (sirStats[k_recovery_time_final]
																			- sirStats[k_recovery_time]);
	}

	return times;
}

float Calc::findMaxInterval(const RecoilTable recoilTable, const RecoveryTimes recoveryTimes) const
{
	float tapInterval = sirStats[k_cycletime];

	const unsigned int clipSize = static_cast<unsigned int>(sirStats[k_primary_clip_size]);
	unsigned int taps;

	do
	{
		float inaccuracy = baseInaccuracy;
		float recoilIndex = 0.f;
		float sumDecayTime = 0.f;
		Recoil recoil{};
		Recoil velocity{};
		size_t ticks = 0u;

		recoil = calcRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
		recoil = { floorTo6(recoil[k_x]), floorTo6(recoil[k_y]) };

		for (taps = 1u; taps < clipSize; ++taps)
		{
			ticks = ticksSinceLastTap(tapInterval, sumDecayTime);

			Recoil newRecoil = calcRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
			float newInaccuracy = floorTo6(calcInaccuracy(inaccuracy, recoveryTimes[ceilToInt(recoilIndex)], ticks / tickrate));

			newRecoil = { floorTo6(newRecoil[k_x]), floorTo6(newRecoil[k_y]) };

			if ((newInaccuracy != inaccuracy || newRecoil != recoil) && taps != 1u) //Skips comparison to base inaccuracy
			{
				tapInterval += params.tapIncr;
				break;
			}

			inaccuracy = newInaccuracy;
			recoil = newRecoil;
		}
	} while (taps != clipSize);

	return tapInterval;
}

Calc::Recoil Calc::calcRecoil(const size_t ticksSinceLastCalc, const RecoilTable recoilTable, float &recoilIndex, Recoil recoil,
							  Recoil &velocity) const
{
	const float decay2Exp = expf(-8.f / tickrate);
	const float decay2Lin = 18.f / tickrate;
	const float velDecay = expf(-4.5f / tickrate);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Decay recoilIndex inside this for loop/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (size_t i = 0u; i < ticksSinceLastCalc; ++i)
	{
		recoil = { recoil[k_x] * decay2Exp, recoil[k_y] * decay2Exp };

		const float recoilMagnitude = sqrtf(powf(recoil[k_x], 2.f) + powf(recoil[k_y], 2.f));

		if (recoilMagnitude > decay2Lin)
		{
			const float linDecay = 1.f - decay2Lin / recoilMagnitude;

			recoil = { recoil[k_x] * linDecay, recoil[k_y] * linDecay };
		}
		else
			recoil = { 0.f, 0.f };

		const float averageConstant = 1.f / tickrate / 2.f;

		recoil = { recoil[k_x] + velocity[k_x] * averageConstant, recoil[k_y] + velocity[k_y] * averageConstant };
		velocity = { velocity[k_x] * velDecay, velocity[k_y] * velDecay };
		recoil = { recoil[k_x] + velocity[k_x] * averageConstant, recoil[k_y] + velocity[k_y] * averageConstant };
	}

	const size_t index = static_cast<size_t>(recoilIndex++);
	const float recoilAngle = recoilTable[index][k_angle] * static_cast<float>(M_PI) / 180.f;

	velocity = { velocity[k_x] + recoilTable[index][k_magnitude] * sinf(recoilAngle),
				 velocity[k_y] + recoilTable[index][k_magnitude] * cosf(recoilAngle) };

	return recoil;
}

size_t Calc::ticksSinceLastTap(const float tapInterval, float &sumDecayTime) const
{
	const float oldTotalDecayTime = sumDecayTime + (sumDecayTime == 0.f ? 1.f / tickrate : 0.f);

	sumDecayTime = floorTo6(sumDecayTime + tapInterval); //Floors to 6 decimal places to avoid accumulated floating-point error

	return ceilToInt(sumDecayTime * tickrate) - ceilToInt(oldTotalDecayTime * tickrate);
}

size_t Calc::ceilToInt(const float inputFloat) const
{
	return static_cast<size_t>(ceilf(inputFloat));
}

float Calc::calcInaccuracy(const float inaccuracy, const float recoveryTime, const float elapsedTime) const
{
	return (sirStats[k_inaccuracy_fire] + inaccuracy - baseInaccuracy) * powf(0.1f, elapsedTime / recoveryTime) + baseInaccuracy;
}

float Calc::tapHitPercent(const float inaccuracy, Random &rand, Recoil recoil) const
{
	constexpr float k_2_pi = static_cast<float>(2. * M_PI);

	const float bboxHalfHeight = bboxCylHalfHeight + bboxRadius;
	const float scaleToDistance = 0.001f * params.distance;
	int hits = 0;

	recoil = { tanf(recoil[k_x] * k_2_pi / 180.f) * params.distance, tanf(recoil[k_y] * k_2_pi / 180.f) * params.distance };

	for (unsigned int i = 0; i < params.simsPerTap; ++i)
	{
		const float randInaccuracy = rand.randomFloat() * (inaccuracy - sirStats[k_spread]);
		const float randAngle1 = rand.randomFloat(0.f, k_2_pi);
		const float randSpread = rand.randomFloat() * sirStats[k_spread];
		const float randAngle2 = rand.randomFloat(0.f, k_2_pi);

		const float spreadX = fabsf(recoil[k_x] - (sinf(randAngle1) * randInaccuracy + sinf(randAngle1) * randSpread)
									* scaleToDistance);
		const float spreadY = fabsf(recoil[k_y] - (cosf(randAngle2) * randInaccuracy + cosf(randAngle2) * randSpread)
									* scaleToDistance);

		if (spreadX <= bboxRadius && spreadY <= bboxCylHalfHeight //Shot falls inside 2d bbox rectangle
			|| spreadY > bboxCylHalfHeight && spreadY <= bboxHalfHeight //Shot falls inside 2d bbox semicircle
			   && spreadX <= sqrt(powf(bboxRadius, 2.f) - powf(spreadY - bboxCylHalfHeight, 2.f)))
			++hits;
	}

	return hits * 100.f / params.simsPerTap;
}

void Calc::fillSirStats(const bool bUseAlt, const int weaponIndex, const SirArchive &sir, const wchar_t stance[])
{
	wstring statNames[] = { L"cycletime", L"primary clip size", L"max player speed", L"recovery time ", L"recovery time  final",
							L"spread", L"inaccuracy ", L"inaccuracy fire", L"inaccuracy move", L"recoil magnitude",
							L"recoil magnitude variance", L"recoil angle variance", L"recoil seed", L"is full auto" };

	for (int i = k_first_stat; i < k_num_sir_stats; ++i)
	{
		if (statNames[i].back() == L' ')
			statNames[i] += stance;
		else if (statNames[i].find(L"  ") != wstring::npos)
			statNames[i].insert(statNames[i].find(L"  ") + 1u, stance);

		if (bUseAlt && (statNames[i] == L"spread" || statNames[i].find(L"inaccuracy") != wstring::npos
						|| statNames[i].find(L"recoil") != wstring::npos && statNames[i] != L"recoil seed"))
			statNames[i] += L" alt";

		sirStats[i] = stof(sir.getDatum(weaponIndex, sir.fetchColumnIndex(statNames[i])));
	}
}

float Calc::floorTo6(const float inputFloat) const
{
	return floorf(inputFloat * 1000000) / 1000000;
}

} //namespace calc
} //namespace csgo
} //namespace sircat
