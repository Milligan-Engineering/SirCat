#pragma once
#define k_FIRST_STAT 40

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
		int modelIndex;
		int weaponIndex;
		bool bCrouch;
		int moveSpeed;
		double hitPercent;
		double distance;
		bool b64Tick;
		bool bUseAlt;

		Params() noexcept : modelIndex(0), weaponIndex(0), bCrouch(false), moveSpeed(0), hitPercent(100.0), distance(0.01),
							b64Tick(false), bUseAlt(false) {};

	};

	Calc(const Params &k_params, const sir::SirArchive &sirArchive);

	Calc(const Calc &othercalc) = delete;

	double tapInterval(const double targetInaccuracy) const;

private:
	enum { k_first_stat, k_cycletime = 0, k_primary_clip_size, k_max_player_speed, k_recovery_time, k_recovery_time_final,
		   k_spread, k_inaccuracy_stance, k_inaccuracy_fire, k_inaccuracy_move, k_recoil_magnitude,
		   k_recoil_magnitude_variance,	k_recoil_angle_variance, k_num_stats };

	double calcNewInaccuracy(const double inaccuracy, const double tapInterval, double &totalDecayTime) const;

	double roundTimeToTick(const double time) const;

	bool bHitPercentInDistribution() const;

	const double k_tickrate;
	const Params k_params;
	double stats[k_num_stats];
};

} //namespace calc
} //namespace csgo
} //namespace sircat
