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
	float tapInterval = sirStats[k_cycletime]; //Starts simulation at max firing speed

	const bool bIsFullAuto = sirStats[k_is_full_auto] == 1.f;
	const RecoilTable recoilTable = generateRecoilTable(bIsFullAuto);
	const RecoveryTimes recoveryTimes = generateRecoveryTimes(bIsFullAuto);
	float totalTapHitPercent = 0.f; //Stores the percentage of shots that landed within hitbox over each simulation
	Random rand;

	rand.setSeed();
	maxInterval = findMaxInterval(recoilTable, recoveryTimes); //Sets max tapInterval that can be incremented to for simulations

	do
	{
		float inaccuracy = baseInaccuracy;
		float recoilIndex = 0.f;
		float sumDecayTime = 0.f;
		Recoil recoil{};
		Recoil velocity{};
		size_t ticks = 0u; //Stores the the number of ticks that have passed between consecutive taps in a simulation

		recoil = updateRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
		totalTapHitPercent = tapHitPercent(inaccuracy, rand, recoil);

		for (unsigned int taps = 1u; taps < params.maxTaps; ++taps)
		{
			ticks = ticksSinceLastTap(tapInterval, sumDecayTime);
			recoil = updateRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
			inaccuracy = updateInaccuracy(inaccuracy, recoveryTimes[ceilToInt(recoilIndex)], ticks / tickrate);
			totalTapHitPercent += tapHitPercent(inaccuracy, rand, recoil);
		}

		totalTapHitPercent = totalTapHitPercent / params.maxTaps; //Calculates average hit percentage for taps

		if (totalTapHitPercent < params.targetHitPercent)
			tapInterval += params.tapIncr;
	} while (totalTapHitPercent < params.targetHitPercent && tapInterval < maxInterval);

	return tapInterval;
}

Calc::Params Calc::getParams() const
{
	return params;
}

void Calc::setParams(Params &inParams, const ArchivePair &archivePair)
{
	fillSirStats(inParams.bUseAlt, inParams.weaponIndex, archivePair.getSir(), inParams.bCrouch ? L"crouch" : L"stand");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Add algorithm to retrieve cvars from game files?///////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cvars = { 8.f, 18.f, 2.f, 0.75f, 4.f, 0.55f, 4.5f };
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
		table[i][k_angle] = sirStats[k_recoil_angle] + rand.randomFloat(-1.f, 1.f) * sirStats[k_recoil_angle_variance];
		table[i][k_magnitude] = sirStats[k_recoil_magnitude] + rand.randomFloat(-1.f, 1.f)
								* sirStats[k_recoil_magnitude_variance];

		if (bIsFullAuto)
		{
			if (i != 0u)
			{
				const size_t last = i - 1u;

				//Smooths out angular variance between consecutive shots in the recoil pattern
				table[i][k_angle] = table[last][k_angle] + (table[i][k_angle] - table[last][k_angle]) * 0.55f;
				table[i][k_magnitude] = table[last][k_magnitude] + (table[i][k_magnitude] - table[last][k_magnitude]) * 0.55f;
			}

			if (i < 4u)
				//Lerps over first four shots to reduce their recoil
				table[i][k_magnitude] *= i / cvars.weapon_recoil_suppression_shots
										 * (1.f - cvars.weapon_recoil_suppression_factor)
										 + cvars.weapon_recoil_suppression_factor;
		}
	}

	for (size_t i = 64u; i < 150u; ++i) //Recoil pattern repeats every 64 shots
		table[i] = table[i - 64u];

	return table;
}

Calc::RecoveryTimes Calc::generateRecoveryTimes(const bool bIsFullAuto) const
{
	RecoveryTimes recoveryTimes;

	recoveryTimes.fill(sirStats[k_recovery_time_final]);

	if (sirStats[k_recovery_time] != sirStats[k_recovery_time_final])
	{
		size_t index = 0u; //Holds the index of the last shot before recovery time lerp
		size_t lerpPoints = 3u;

		if (bIsFullAuto)
		{
			index += 2u;

			if (sirStats[k_recovery_time] > sirStats[k_recovery_time_final]) //Evaluates to true if weapon is Negev
				index += 7u;
		}
		else
			lerpPoints += 2u;

		for (size_t i = 0u; i <= index; ++i) //Sets recovery times before recovery time lerp
			recoveryTimes[i] = sirStats[k_recovery_time];

		for (size_t lerpNum = 1u; lerpNum < lerpPoints; ++lerpNum) //Lerps recovery time
			recoveryTimes[index + lerpNum] = sirStats[k_recovery_time] + (sirStats[k_recovery_time_final]
																		  - sirStats[k_recovery_time]) * lerpNum / lerpPoints;
	}

	return recoveryTimes;
}

float Calc::findMaxInterval(const RecoilTable recoilTable, const RecoveryTimes recoveryTimes) const
{
	float tapInterval = sirStats[k_cycletime];

	const int clipSize = static_cast<int>(sirStats[k_primary_clip_size]);
	int taps;

	do
	{
		float inaccuracy = baseInaccuracy;
		float recoilIndex = 0.f;
		float sumDecayTime = 0.f;
		Recoil recoil{};
		Recoil velocity{};
		size_t ticks = 0u;

		recoil = updateRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
		recoil = { floor6(recoil[k_x]), floor6(recoil[k_y]) };

		for (taps = 1; taps < clipSize; ++taps)
		{
			ticks = ticksSinceLastTap(tapInterval, sumDecayTime);

			Recoil newRecoil = updateRecoil(ticks, recoilTable, recoilIndex, recoil, velocity);
			float newInaccuracy = floor6(updateInaccuracy(inaccuracy, recoveryTimes[ceilToInt(recoilIndex)], ticks / tickrate));

			newRecoil = { floor6(newRecoil[k_x]), floor6(newRecoil[k_y]) };

			if ((newInaccuracy != inaccuracy || newRecoil != recoil) && taps != 1) //Skips comparison to base inaccuracy
			{
				tapInterval += params.tapIncr;
				break;
			}

			inaccuracy = newInaccuracy;
			recoil = newRecoil;
		}
	} while (taps != clipSize); //Loops until the foor loop inaccuracy and recoil have converged for the entire clip

	return tapInterval;
}

Calc::Recoil Calc::updateRecoil(const size_t ticksSinceLastCalc, const RecoilTable recoilTable, float &recoilIndex,
								Recoil recoil, Recoil &velocity) const
{
	const float decay2Exp = expf(-cvars.weapon_recoil_decay2_exp / tickrate);
	const float decay2Lin = cvars.weapon_recoil_decay2_lin / tickrate;
	const float velDecay = expf(-cvars.weapon_recoil_vel_decay / tickrate);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Decay recoilIndex inside this for loop/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (size_t i = 0u; i < ticksSinceLastCalc; ++i) //Decays recoil each tick
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

		//Smooths recoil vecolity between consecutive ticks
		recoil = { recoil[k_x] + velocity[k_x] * averageConstant, recoil[k_y] + velocity[k_y] * averageConstant };
		velocity = { velocity[k_x] * velDecay, velocity[k_y] * velDecay };
		recoil = { recoil[k_x] + velocity[k_x] * averageConstant, recoil[k_y] + velocity[k_y] * averageConstant };
	}

	const size_t index = static_cast<size_t>(recoilIndex++);
	const float recoilAngle = recoilTable[index][k_angle] * static_cast<float>(M_PI) / 180.f;

	//Adds velocity from recoil table based on this shot's recoil index
	velocity = { velocity[k_x] + recoilTable[index][k_magnitude] * sinf(recoilAngle),
				 velocity[k_y] + recoilTable[index][k_magnitude] * cosf(recoilAngle) };

	return recoil;
}

size_t Calc::ticksSinceLastTap(const float tapInterval, float &sumDecayTime) const
{
	//Accounts for the simulations first tick shot calculations
	const float oldTotalDecayTime = sumDecayTime + (sumDecayTime == 0.f ? 1.f / tickrate : 0.f);

	sumDecayTime = floor6(sumDecayTime + tapInterval); //Floors to 6 decimal places to avoid accumulated floating-point error

	return ceilToInt(sumDecayTime * tickrate) - ceilToInt(oldTotalDecayTime * tickrate);
}

size_t Calc::ceilToInt(const float inputFloat) const
{
	return static_cast<size_t>(ceilf(inputFloat));
}

float Calc::updateInaccuracy(const float inaccuracy, const float recoveryTime, const float elapsedTime) const
{
	return (sirStats[k_inaccuracy_fire] + inaccuracy - baseInaccuracy) * powf(0.1f, elapsedTime / recoveryTime) + baseInaccuracy;
}

float Calc::tapHitPercent(const float inaccuracy, Random &rand, Recoil recoil) const
{
	constexpr float k_2_pi = static_cast<float>(2. * M_PI);

	const float bboxHalfHeight = bboxCylHalfHeight + bboxRadius;
	const float scaleToDistance = 0.001f * params.distance; //Scales shot spread to x/y coordinates based on distance from target
	int hits = 0;
	const float radiansScale = cvars.weapon_recoil_scale * static_cast<float>(M_PI) / 180.f;

	//Converts view punch angles and scales shot recoil to x/y coordinates based on distance from target
	recoil = { tanf(recoil[k_x] * radiansScale) * params.distance, tanf(recoil[k_y] * radiansScale) * params.distance };

	for (unsigned int i = 0; i < params.simsPerTap; ++i)
	{
		const float randInaccuracy = rand.randomFloat() * (inaccuracy - sirStats[k_spread]);
		const float randAngle1 = rand.randomFloat(0.f, k_2_pi);
		const float randSpread = rand.randomFloat() * sirStats[k_spread];
		const float randAngle2 = rand.randomFloat(0.f, k_2_pi);

		const float xCoordinate = fabsf(recoil[k_x] - (sinf(randAngle1) * randInaccuracy + sinf(randAngle1) * randSpread)
									* scaleToDistance);
		const float yCoordinate = fabsf(recoil[k_y] - (cosf(randAngle2) * randInaccuracy + cosf(randAngle2) * randSpread)
									* scaleToDistance);

		if (xCoordinate <= bboxRadius && yCoordinate <= bboxCylHalfHeight
			|| yCoordinate > bboxCylHalfHeight && yCoordinate <= bboxHalfHeight
			   && xCoordinate <= sqrt(powf(bboxRadius, 2.f) - powf(yCoordinate - bboxCylHalfHeight, 2.f)))
			++hits; //Registers shots whose coordinates were within the 2d bbox's rectangle or semicircle
	}

	return hits * 100.f / params.simsPerTap;
}

void Calc::fillSirStats(const bool bUseAlt, const int weaponIndex, const SirArchive &sir, const wchar_t stance[])
{
	wstring statNames[] = { L"cycletime", L"primary clip size", L"max player speed", L"recovery time ", L"recovery time  final",
							L"spread", L"inaccuracy ", L"inaccuracy fire", L"inaccuracy move", L"recoil magnitude",
							L"recoil magnitude variance", L"recoil angle", L"recoil angle variance", L"recoil seed",
							L"is full auto" };

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

float Calc::floor6(const float inputFloat) const
{
	return floorf(inputFloat * 1000000) / 1000000;
}

} //namespace calc
} //namespace csgo
} //namespace sircat
