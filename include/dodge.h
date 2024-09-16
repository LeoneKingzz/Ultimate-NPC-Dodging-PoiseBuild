#pragma once
//#include <unordered_set>
#include <algorithm>
#include "RE/M/Misc.h"
#include "lib/PrecisionAPI.h"


using PRECISION_API::PreHitCallback;
using std::string;

enum dodge_direction
{
	kForward = 1,
	kRightForward = 2,
	kRight = 3,
	kRightBackward = 4,
	kBackward = 5,
	kLeftBackward = 6,
	kLeft = 7,
	kLeftForward = 8
};
typedef std::vector<dodge_direction> dodge_dir_set;
typedef float (*tActor_GetReach)(RE::Actor* a_this);
static REL::Relocation<tActor_GetReach> Actor_GetReach{ RELOCATION_ID(37588, 38538) };

const dodge_dir_set dodge_directions_tk_horizontal = { dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_back = { dodge_direction::kBackward };
const dodge_dir_set dodge_directions_tk_vertical = { dodge_direction::kForward, dodge_direction::kBackward };
const dodge_dir_set dodge_directions_tk_all = { dodge_direction::kForward, dodge_direction::kBackward, dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_reactive = { dodge_direction::kBackward, dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_ranged = { dodge_direction::kForward, dodge_direction::kLeft, dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_right = { dodge_direction::kRight };
const dodge_dir_set dodge_directions_tk_left = { dodge_direction::kLeft };

const dodge_dir_set dodge_directions_dmco_horizontal = { dodge_direction::kLeft, dodge_direction::kRight, dodge_direction::kLeftForward, dodge_direction::kRightForward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_left = { dodge_direction::kLeft, dodge_direction::kLeftForward, dodge_direction::kLeftBackward };
const dodge_dir_set dodge_directions_dmco_right = { dodge_direction::kRight, dodge_direction::kRightForward, dodge_direction::kRightBackward };

const dodge_dir_set dodge_directions_dmco_back = { dodge_direction::kBackward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_vertical = { dodge_direction::kForward, dodge_direction::kBackward, dodge_direction::kLeftForward, dodge_direction::kRightForward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_reactive = { dodge_direction::kLeft, dodge_direction::kRight, dodge_direction::kBackward, dodge_direction::kLeftBackward, dodge_direction::kRightBackward };
const dodge_dir_set dodge_directions_dmco_all = {
	dodge_direction::kForward,
	dodge_direction::kBackward,
	dodge_direction::kLeft,
	dodge_direction::kRight,
	dodge_direction::kLeftForward,
	dodge_direction::kRightForward,
	dodge_direction::kLeftBackward,
	dodge_direction::kRightBackward
};

class dodge
{
private:
	std::random_device rd;

public:

	void init();

	static dodge* GetSingleton()
	{
		static dodge singleton;
		return &singleton;
	}

	struct Armour_factors
	{
		float Heavyarm_mult = 0.25f;
		float Lightarm_mult = 0.75f;
		float clothing_mult = 1.0f;

		float Helm_weight = 0.1f;
		float Gauntlet_weight = 0.1f;
		float Boots_weight = 0.1f;
		float Chest_weight = 0.6f;
		float Shield_weight = 0.1f;

	} Armourr;

	struct PReflex_factors
	{
		float Sneak_Weighting = 0.2f;
		float Skirmish_Weighting = 0.2f;
		float Defensive_Weighting = 0.3f;
		float Armour_Weighting = 0.3f;

	} Protagnist_Reflexess;

	struct CStyle_factors
	{
		float Skirmish_AvoidThreat_Weighting = 0.25f;
		float Skirmish_Fallback_Weighting = 0.3f;
		float Skirmish_Circle_Weighting = 0.3f;
		float Skirmish_Strafe_Weighting = 0.15f;

	} CStylee;

	struct Stamina_factors
	{
		float Stamina_HeavyArmour_mult = 0.15f;
		float Stamina_LightArmour_mult = 0.7f;
		float Stamina_Clothing_mult = 1.0f;

		float Stamina_Helm_weight = 0.1f;
		float Stamina_Gauntlet_weight = 0.1f;
		float Stamina_Boots_weight = 0.1f;
		float Stamina_Chest_weight = 0.6f;
		float Stamina_Shield_weight = 0.1f;

		float fSideStep_staminacost = 25.0f;  
		float fDodgeRoll_staminacost = 1.6f; 
	} Staminaa;

	float get_dodge_chance(RE::Actor* a_actor, const Armour_factors& Armour, const PReflex_factors& Protagnist_Reflexes, const CStyle_factors& CStyle);
	float get_stamina_basecost(RE::Actor* a_actor, const Stamina_factors& Stamina, bool DodgeRoll = false);

	void react_to_melee(RE::Actor* a_attacker, float attack_range);
	void react_to_melee_power(RE::Actor* a_attacker, float attack_range);
	void react_to_melee_normal(RE::Actor* a_attacker, float attack_range);
	void react_to_bash(RE::Actor* a_attacker, float attack_range);
	void react_to_bash_sprint(RE::Actor* a_attacker, float attack_range);
	void react_to_ranged(RE::Actor* a_attacker, float attack_range);
	void react_to_shouts_spells(RE::Actor* a_attacker, float attack_range);
	void react_to_shouts_spells_fast(RE::Actor* a_attacker, float attack_range, bool lefthand = false);


	static float GetProtaganist_ReflexScore(RE::Actor* a_actor);

	// static bool Protagnist_can_dodge(RE::Actor* a_actor);

	static bool BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm);

	int GenerateRandomInt(int value_a, int value_b);
	float GenerateRandomFloat(float value_a, float value_b);
		
	void attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void Powerattack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void NormalAttack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void Shouts_Spells_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void Bash_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);
	void BashSprint_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge = false);

	static void Set_iFrames(RE::Actor* actor);
	static void Reset_iFrames(RE::Actor* actor);

	//void set_dodge_phase(RE::Actor* a_dodger, bool a_isDodging);
	//bool get_is_dodging(RE::Actor* a_actor);

	float Get_ReactiveDodge_Distance(RE::Actor *actor);
	float Get_ReactiveDodge_Reach(RE::Actor *actor);
	bool GetAttackSpell(RE::Actor* actor, bool lefthand = false);
	bool GetAttackSpell_Alt(RE::SpellItem* a_spell);
	bool GetEquippedShout(RE::Actor* actor);
	float GetShoutRange_Reaction(RE::Actor* actor, float distance);
	float GetSpellRange_Reaction(RE::Actor* actor, float distance, bool lefthand = false);
	void send_UNDdodge__event(RE::Actor *a_attacker);
	static bool getrace_VLserana(RE::Actor *a_actor);
	
	

private:

	PRECISION_API::IVPrecision1* _precision_API;

	static PRECISION_API::PreHitCallbackReturn DodgeCallback_PreHit(const PRECISION_API::PrecisionHitData& a_precisionHitData);

	bool able_dodge(RE::Actor* a_actor);

	bool can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest);

	void do_dodge(RE::Actor* a_actor, dodge_direction a_direction);
	void TRKE_dodge(RE::Actor* actor, const char* a_event, bool backingoff = false);

	RE::NiPoint3 get_dodge_vector(dodge_direction a_direction);
	void do_dodge_VLSerana(RE::Actor* a_actor, dodge_direction a_direction);

	//std::unordered_set<RE::ActorHandle> dodging_actors;
	//mutable std::shared_mutex dodging_actors_lock;
	
};