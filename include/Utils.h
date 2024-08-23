#pragma once
#include <cmath>
#include  <random>
#include  <iterator>
#include "PCH.h"
#include "offsets.h"
#include <shared_mutex>


#define CONSOLELOG(msg) RE::ConsoleLog::GetSingleton()->Print(msg);
#define PI 3.1415926535897932384626f


namespace Utils
{
	template <typename Iter, typename RandomGenerator>
	Iter select_randomly(Iter start, Iter end, RandomGenerator& g)
	{
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template <typename Iter>
	Iter select_randomly(Iter start, Iter end)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return select_randomly(start, end, gen);
	}

	template <int id, typename T, typename... Args>
	T _generic_foo(Args... args)
	{
		using func_t = T(Args...);
		REL::Relocation<func_t> func{REL::ID(id)};
		return func(std::forward<Args>(args)...);
	}

	void queueMessageBox(RE::BSFixedString a_message);

	void playSound(RE::Actor* a, RE::BGSSoundDescriptorForm* a_descriptor, float a_volumeOverride = 1);
	
	RE::NiPoint3 get_abs_pos(RE::Actor* a_actor, RE::NiPoint3 a_relative_pos);

	void PushActorAway(RE::Actor* causer, RE::Actor* target, float magnitude);

	void SetRotationMatrix(RE::NiMatrix3 &a_matrix, float sacb, float cacb, float sb);

	void slowTime(float a_duration, float a_percentage);

	RE::BGSAttackData *get_attackData(RE::Actor *a);

	struct PolarAngle
	{
		float alpha;
		operator float() const { return alpha; }
		PolarAngle(float x = 0.0f) : alpha(x)
		{
			while (alpha > 360.0f)
				alpha -= 360.0f;
			while (alpha < 0.0f)
				alpha += 360.0f;
		}
		PolarAngle(const RE::NiPoint3 &p)
		{
			const float y = p.y;
			if (y == 0.0)
			{
				if (p.x <= 0.0)
					alpha = PI * 1.5f;
				else
					alpha = PI * 0.5f;
			}
			else
			{
				alpha = atanf(p.x / y);
				if (y < 0.0)
					alpha += PI;
			}
			alpha = alpha * 180.0f / PI;
		}
		PolarAngle add(const PolarAngle &r) const
		{
			float ans = alpha + r.alpha;
			if (ans >= 360.0f)
				return {ans - 360.0f};
			else
				return {ans};
		}
		PolarAngle sub(const PolarAngle &r) const
		{
			return this->add({360.0f - r.alpha});
		}
		float to_normangle() const
		{
			if (alpha > 180.0f)
				return alpha - 360.0f;
			else
				return alpha;
		}
		float to_normangle_abs() const
		{
			return abs(to_normangle());
		}
		static bool ordered(PolarAngle alpha, PolarAngle beta, PolarAngle gamma)
		{
			gamma = gamma.sub(alpha);
			beta = beta.sub(alpha);
			return gamma >= beta;
		}
		static float dist(float r, PolarAngle alpha)
		{
			auto ans = r * (alpha) / 180.0f * PI;
			return ans;
		}
	};

	float get_angle_he_me(RE::Actor *me, RE::Actor *he, RE::BGSAttackData *attackdata);

	namespace Actor
	{
		RE::TESObjectWEAP* getWieldingWeapon(RE::Actor* a_actor);
		bool isDualWielding(RE::Actor* a_actor);
		bool isEquippedShield(RE::Actor* a_actor);
		bool isPowerAttacking(RE::Actor* a_actor);
		bool isBashing(RE::Actor* a_actor);
		bool isHumanoid(RE::Actor* a_actor);
		void getBodyPos(RE::Actor* a_actor, RE::NiPoint3& pos);
		void getHeadPos(RE::Actor* a_actor, RE::NiPoint3& pos);
		void getHeadPos(RE::Actor* a_actor, RE::NiAVObject*& pos);
		void dropShield(RE::Actor* a_actor);
	}

	namespace math
	{
#define PI 3.14159265358979323846f
#define TWO_PI 6.28318530717958647692f
		float NormalRelativeAngle(float a_angle);
	}
	
};

/*Utilities related to combat*/
namespace ValhallaUtils
{
	bool is_adversary(RE::Actor* actor1, RE::Actor* actor2);
	bool isBackFacing(RE::Actor* actor1, RE::Actor* actor2);

};

/*Some of my own convenience stuff*/
namespace DtryUtils
{
	class rayCast
	{

#define YELLOW 0xFFFF00      // yellow
#define GREEN 0xff00ff   //green
	public:

		/*Do a simple raycast at a singular point to check if anything exists there.
		If anything exists, update A_POS argument to the position where raycast is hit.*/
		static bool object_exists(RE::NiPoint3& a_pos, float a_range = 30.f);
		/*Cast a ray from the center of the actor to a_rayEnd, return the first object encountered, or nullptr if nothing is hit.*/
		static RE::TESObjectREFR* cast_ray(RE::Actor* a_actor, RE::NiPoint3 a_rayEnd, float a_castPos = 0.5f, float* ret_rayDist = nullptr);
				
		
	};
}




constexpr uint32_t hash(const char* data, size_t const size) noexcept
{
	uint32_t hash = 5381;

	for (const char* c = data; c < data + size; ++c) {
		hash = ((hash << 5) + hash) + (unsigned char)*c;
	}

	return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
	return hash(str, size);
}

