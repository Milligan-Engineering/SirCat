#pragma once

namespace sircat {
namespace csgo {

namespace sir { class SirData; }

class Calc
{
public:
	enum class WhichParam { ALL, MODEL_INDEX, WEAPON_INDEX, B_USE_ALT, B_CROUCH, MOVE_SPEED, HIT_PERCENT, DISTANCE, B_64_TICK };

	struct Params
	{
		int modelIndex;
		int weaponIndex;
		bool bUseAlt;
		bool bCrouch;
		int moveSpeed;
		double hitPercent;
		double distance;
		bool b64Tick;

		Params() : bUseAlt(false), bCrouch(false), moveSpeed(0), hitPercent(100.0), b64Tick(false) {}; //temp ////////////////////////////////////////////////
	};

	Calc() = delete;
	Calc(const Params &params, const sir::SirData &sirData);
	~Calc() = default;

	double tapInterval(const double targetInaccuracy) const;
private:
	struct Stats
	{
		enum I { CYCLETIME, PRIMARY_CLIP_SIZE, MAX_PLAYER_SPEED, RECOVERY_TIME, RECOVERY_TIME_FINAL, SPREAD, INACCURACY_STANCE,
			   INACCURACY_FIRE, INACCURACY_MOVE, RECOIL_MAGNITUDE, RECOIL_MAGNITUDE_VARIANCE, RECOIL_ANGLE_VARIANCE, NUM_STATS };
	};

	double calcNewInaccuracy(const double inaccuracy, const double tapInterval, double &totalDecayTime) const;

	const double tickrate;
	double stats[Stats::NUM_STATS];
};

} //namespace csgo
} //namespace sircat
