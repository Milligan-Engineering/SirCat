#pragma once

#include <array>
#include <string>

namespace sircat {
namespace csgo {

namespace sir { class SirArchive; }

namespace calc {

class Calc
{
public:
	enum class WhichParam { k_all, k_model_index, k_weapon_index, k_b_crouch, k_move_speed, k_hit_percent, k_distance,
							k_b_64_tick, k_b_use_alt };

	struct Params
	{
		int modelIndex = 0;
		int weaponIndex = 0;
		bool bCrouch = false;
		double moveSpeed = 0.0;
		double hitPercent = 100.0;
		double distance = 0.0;
		bool b64Tick = false;
		bool bUseAlt = false;
	};

	Calc() = default;

	double tapInterval(const double targetInaccuracy) const;

	const Params &getParams() const;

	void setParams(const Params &newParams, const sir::SirArchive &sirArchive);

private:
	enum { k_first_stat, k_cycletime = 0, k_primary_clip_size, k_max_player_speed, k_recovery_time, k_recovery_time_final,
		   k_spread, k_inaccuracy_stance, k_inaccuracy_fire, k_inaccuracy_move, k_recoil_magnitude,
		   k_recoil_magnitude_variance,	k_recoil_angle_variance, k_num_stats };

	double calcNewInaccuracy(const double inaccuracy, const double tapInterval, double &totalDecayTime) const;

	double roundTimeToTick(const double time) const;

	bool bHitPercentInDistribution() const;

	double stats[k_num_stats]{ 0.0 };
	double tickrate = 0.0;
	Params params;
	std::array<std::wstring, 12> partialStatNames{ L"cycletime", L"primary clip size", L"max player speed", L"recovery time ",
												   L"recovery time  final", L"spread", L"inaccuracy ", L"inaccuracy fire",
												   L"inaccuracy move", L"recoil magnitude", L"recoil magnitude variance",
												   L"recoil angle variance" };
	std::array<std::wstring, 12> statNames = partialStatNames;
};

} //namespace calc
} //namespace csgo
} //namespace sircat
