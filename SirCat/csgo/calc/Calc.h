#pragma once

#include <array>	//Using std::array
#include <cstddef>	//Using std::size_t

namespace sircat {
namespace csgo {

class ArchivePair;

namespace sir { class SirArchive; }
namespace calc {

class Random;

class Calc
{
public:
	enum class WhichParam { k_all, k_model_index, k_weapon_index, k_b_crouch, k_move_speed, k_hit_percent, k_distance,
							k_b_64_tick, k_b_use_alt, k_max_taps, k_tap_incr, k_sims_per_tap };

	struct Params
	{
		unsigned int modelIndex;
		unsigned int weaponIndex;
		bool bCrouch;
		float moveSpeed;
		float hitPercent;
		float distance;
		bool b64Tick;
		bool bUseAlt;
		unsigned int maxTaps;
		float tapIncr;
		unsigned int simsPerTap;
	};

	float tapInterval(float &maxInterval) const;

	Params getParams() const;

	void setParams(Params &inParams, const ArchivePair &archivePair);

private:
	typedef std::array<float, 2u> Recoil;
	typedef std::array<Recoil, 150u> RecoilTable;
	typedef std::array<float, 150u> RecoveryTimes;

	enum { k_cycletime, k_primary_clip_size, k_max_player_speed, k_recovery_time, k_recovery_time_final, k_spread,
		   k_inaccuracy_stance, k_inaccuracy_fire, k_inaccuracy_move, k_recoil_magnitude, k_recoil_magnitude_variance,
		   k_recoil_angle_variance, k_recoil_seed, k_is_full_auto, k_num_sir_stats, k_first_stat = 0,
		   k_bbminx = 0, k_bbminy, k_bbminz, k_bbmaxx, k_bbmaxy, k_bbmaxz, k_radius,
		   k_angle = 0, k_magnitude,
		   k_x = 0, k_y };

	RecoilTable generateRecoilTable(const bool bIsFullAuto) const;

	RecoveryTimes generateRecoveryTimes(const bool bIsFullAuto) const;

	float findMaxInterval(const RecoilTable recoilTable, const RecoveryTimes recoveryTimes) const;

	Recoil calcRecoil(const std::size_t ticksSinceLastCalc, const RecoilTable recoilTable, float &recoilIndex, Recoil recoil,
					  Recoil &velocity) const;

	std::size_t ticksSinceLastTap(const float tapInterval, float &sumDecayTime) const;

	std::size_t ceilToInt(const float inputFloat) const;

	float calcInaccuracy(const float inaccuracy, const float recoveryTime, const float elapsedTime) const;

	float tapHitPercent(const float inaccuracy, Random &rand, Recoil recoil) const;

	void fillSirStats(const bool bUseAlt, const int weaponIndex, const sir::SirArchive &sir, const wchar_t stance[]);

	float floorTo6(const float inputFloat) const;

	float baseInaccuracy;
	float bboxCylHalfHeight;
	float bboxRadius;
	float sirStats[k_num_sir_stats];
	float tickrate;
	Params params;
};

} //namespace calc
} //namespace csgo
} //namespace sircat
