#include "settings.h"
#pragma warning(disable: 4100)
#include <chrono>
#include <iostream>
#include <windows.h>

// using Clock = std::chrono::steady_clock;
// std::chrono::time_point<std::chrono::steady_clock> start, now;
// std::chrono::milliseconds                          duration;

//#define PI 3.1415926535f
using writeLock = std::unique_lock<std::shared_mutex>;
using readLock = std::shared_lock<std::shared_mutex>;


void dodge::init() {
	_precision_API = reinterpret_cast<PRECISION_API::IVPrecision1*>(PRECISION_API::RequestPluginAPI());
	if (_precision_API) {
		_precision_API->AddPreHitCallback(SKSE::GetPluginHandle(), DodgeCallback_PreHit);
		logger::info("Enabled compatibility with Precision");
	}
}

void interruptattack(RE::Actor *me)
{
	me->NotifyAnimationGraph("attackStop");
	me->NotifyAnimationGraph("bashStop");
	me->NotifyAnimationGraph("blockStop");
	me->NotifyAnimationGraph("staggerStop");
	me->NotifyAnimationGraph("recoilStop");
}

//Native Functions for Papyrus
float dodge::GetProtaganist_ReflexScore(RE::Actor* a_actor){
	float Score = 0.0f;

	return Score;
}

bool dodge::BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm)
{
	vm->RegisterFunction("GetProtaganist_ReflexScore", "_SM_UND_NativeFunctions", GetProtaganist_ReflexScore);
	// vm->RegisterFunction("Protagnist_can_dodge", "_SM_UND_NativeFunctions", Protagnist_can_dodge);
	return true;
}

PRECISION_API::PreHitCallbackReturn dodge::DodgeCallback_PreHit(const PRECISION_API::PrecisionHitData& a_precisionHitData)
{
	PRECISION_API::PreHitCallbackReturn returnData;
	if (!a_precisionHitData.target || !a_precisionHitData.target->Is(RE::FormType::ActorCharacter)) {
		return returnData;
	}

	auto actor = a_precisionHitData.target->As<RE::Actor>();

	if (actor->IsPlayerRef()) {
		return returnData;
	}

	if (!Utils::Actor::isHumanoid(actor)) {
		return returnData;
	}

	if (!ValhallaUtils::is_adversary(actor, a_precisionHitData.attacker)) {
		return returnData;
	}

	bool bIsDodging = false;

	if ((actor)
			->GetGraphVariableBool("bIsDodging", bIsDodging) &&
		bIsDodging) {
		returnData.bIgnoreHit = true;
	}

	return returnData;
}

/*Get the dodge chance of a reactive dodger in case of an incoming attack.*/
float dodge::get_dodge_chance(RE::Actor* a_actor, const Armour_factors& Armour, const PReflex_factors& Protagnist_Reflexes, const CStyle_factors& CStyle)
{
	float Score = 0.0f;

	/////////////////////////////////////////////////Armour Weighting////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	auto Helm = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHair);

	auto Chest = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody);

	auto Gauntlet = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHands);

	auto Boots = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet);

	auto Shield = a_actor->GetEquippedObject(true);

	if (Helm) {
		switch (Helm->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Helm_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Helm_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Helm_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		default:
			Score += Armour.Helm_weight * Protagnist_Reflexes.Armour_Weighting;
		}
	} else {
		Score += Armour.Helm_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Chest) {
		switch (Chest->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Chest_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Chest_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Chest_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		default:
			Score += Armour.Chest_weight * Protagnist_Reflexes.Armour_Weighting;
		}
	} else {
		Score += Armour.Chest_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Gauntlet) {
		switch (Gauntlet->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Gauntlet_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Gauntlet_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Gauntlet_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		default:
			Score += Armour.Gauntlet_weight * Protagnist_Reflexes.Armour_Weighting;
		}
	} else {
		Score += Armour.Gauntlet_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Boots) {
		switch (Boots->GetArmorType()) {
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Boots_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Boots_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Boots_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		default:
			Score += Armour.Boots_weight * Protagnist_Reflexes.Armour_Weighting;
		}
	} else {
		Score += Armour.Boots_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	if (Shield && Shield->IsArmor()) {
		// if (Shield->HasKeywordByEditorID())
		switch (Shield->As<RE::TESObjectARMO>()->GetArmorType()) {  //function tests for biped model; need some king of flag or keyword instead for sheilds, else crash
		case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
			Score += Armour.Shield_weight * Armour.Heavyarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kLightArmor:
			Score += Armour.Shield_weight * Armour.Lightarm_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		case RE::BIPED_MODEL::ArmorType::kClothing:
			Score += Armour.Shield_weight * Armour.clothing_mult * Protagnist_Reflexes.Armour_Weighting;
			break;
		default:
			Score += Armour.Shield_weight * Protagnist_Reflexes.Armour_Weighting;
			break;
		}
	} else {
		Score += Armour.Shield_weight * Protagnist_Reflexes.Armour_Weighting;
	}

	/////////////////////////////////////////////////Defensive & Skirmish Weighting ///////////////////////////////////////////////////////////////////////////////////////////

	if (a_actor->GetActorRuntimeData().combatController) {
		RE::TESCombatStyle* style = a_actor->GetActorRuntimeData().combatController->combatStyle;
		if (style) {
			Score += style->generalData.defensiveMult * Protagnist_Reflexes.Defensive_Weighting;

			Score += style->generalData.avoidThreatChance * CStyle.Skirmish_AvoidThreat_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->closeRangeData.circleMult * CStyle.Skirmish_Circle_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->closeRangeData.fallbackMult * CStyle.Skirmish_Fallback_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
			Score += style->longRangeData.strafeMult * CStyle.Skirmish_Strafe_Weighting * Protagnist_Reflexes.Skirmish_Weighting;
		}
	}

	/////////////////////////////////////////////////Sneak Skill Weighting /////////////////////////////////////////////////////////////////////////////////////////////////////////

	Score += (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kSneak) / 100.0f) * Protagnist_Reflexes.Sneak_Weighting;

	return Score;
}

float dodge::get_stamina_basecost(RE::Actor* a_actor, const Stamina_factors& Stamina, bool DodgeRoll)
{
	float A_Score = Stamina.fSideStep_staminacost;

	if (settings::bStaminaCost_ArmourBasedSystem_enable){
		auto Helm = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHair);

		auto Chest = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody);

		auto Gauntlet = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHands);

		auto Boots = a_actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kFeet);

		auto Shield = a_actor->GetEquippedObject(true);

		auto Heavy_Skill = (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHeavyArmor) / 100.0f);

		auto Light_skill = (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kLightArmor) / 100.0f);

		if (Helm) {
			switch (Helm->GetArmorType()) {
			case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
				A_Score += (Helm->GetWeight() * Stamina.Stamina_Helm_weight * Stamina.Stamina_HeavyArmour_mult) / Heavy_Skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kLightArmor:
				A_Score += (Helm->GetWeight() * Stamina.Stamina_Helm_weight * Stamina.Stamina_LightArmour_mult) / Light_skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kClothing:
				A_Score += (Helm->GetWeight() * Stamina.Stamina_Helm_weight * Stamina.Stamina_Clothing_mult);
				break;
			default:
				A_Score += (Helm->GetWeight() * Stamina.Stamina_Helm_weight * Stamina.Stamina_Clothing_mult);
			}
		}

		if (Chest) {
			switch (Chest->GetArmorType()) {
			case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
				A_Score += (Chest->GetWeight() * Stamina.Stamina_Chest_weight * Stamina.Stamina_HeavyArmour_mult) / Heavy_Skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kLightArmor:
				A_Score += (Chest->GetWeight() * Stamina.Stamina_Chest_weight * Stamina.Stamina_LightArmour_mult) / Light_skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kClothing:
				A_Score += (Chest->GetWeight() * Stamina.Stamina_Chest_weight * Stamina.Stamina_Clothing_mult);
				break;
			default:
				A_Score += (Chest->GetWeight() * Stamina.Stamina_Chest_weight * Stamina.Stamina_Clothing_mult);
			}
		}

		if (Gauntlet) {
			switch (Gauntlet->GetArmorType()) {
			case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
				A_Score += (Gauntlet->GetWeight() * Stamina.Stamina_Gauntlet_weight * Stamina.Stamina_HeavyArmour_mult) / Heavy_Skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kLightArmor:
				A_Score += (Gauntlet->GetWeight() * Stamina.Stamina_Gauntlet_weight * Stamina.Stamina_LightArmour_mult) / Light_skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kClothing:
				A_Score += (Gauntlet->GetWeight() * Stamina.Stamina_Gauntlet_weight * Stamina.Stamina_Clothing_mult);
				break;
			default:
				A_Score += (Gauntlet->GetWeight() * Stamina.Stamina_Gauntlet_weight * Stamina.Stamina_Clothing_mult);
			}
		}

		if (Boots) {
			switch (Boots->GetArmorType()) {
			case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
				A_Score += (Boots->GetWeight() * Stamina.Stamina_Boots_weight * Stamina.Stamina_HeavyArmour_mult) / Heavy_Skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kLightArmor:
				A_Score += (Boots->GetWeight() * Stamina.Stamina_Boots_weight * Stamina.Stamina_LightArmour_mult) / Light_skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kClothing:
				A_Score += (Boots->GetWeight() * Stamina.Stamina_Boots_weight * Stamina.Stamina_Clothing_mult);
				break;
			default:
				A_Score += (Boots->GetWeight() * Stamina.Stamina_Boots_weight * Stamina.Stamina_Clothing_mult);
			}
		}

		if (Shield && Shield->IsArmor()) {
			// if (Shield->HasKeywordByEditorID())
			switch (Shield->As<RE::TESObjectARMO>()->GetArmorType()) {  //function tests for biped model; need some king of flag or keyword instead for sheilds, else crash
			case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
				A_Score += (Shield->GetWeight() * Stamina.Stamina_Shield_weight * Stamina.Stamina_HeavyArmour_mult) / Heavy_Skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kLightArmor:
				A_Score += (Shield->GetWeight() * Stamina.Stamina_Shield_weight * Stamina.Stamina_LightArmour_mult) / Light_skill;
				break;
			case RE::BIPED_MODEL::ArmorType::kClothing:
				A_Score += (Shield->GetWeight() * Stamina.Stamina_Shield_weight * Stamina.Stamina_Clothing_mult);
				break;
			default:
				A_Score += (Shield->GetWeight() * Stamina.Stamina_Shield_weight * Stamina.Stamina_Clothing_mult);
			}
		}
	}

	
	if (DodgeRoll){
		if (Stamina.fSideStep_staminacost == 0.0) {
			return A_Score * Stamina.fSideStep_staminacost;

		} else {
			return A_Score * Stamina.fDodgeRoll_staminacost;
		}

	}else {
		if(Stamina.fSideStep_staminacost == 0.0) {
			return A_Score * Stamina.fSideStep_staminacost;

		} else {
			return A_Score;
		}
	}
}

float dodge::Get_ReactiveDodge_Distance(RE::Actor *actor) {

	float distance = 200.0f;
	
	auto aiProcess = actor->GetActorRuntimeData().currentProcess;

	if (aiProcess && aiProcess->high && aiProcess->high->attackData) {
		// const RE::TESForm* 
		auto equippedLi = aiProcess->high->attackData.get();
		if (equippedLi) {
			const RE::TESForm* equipped = equippedLi->IsLeftAttack() ? aiProcess->GetEquippedLeftHand() : aiProcess->GetEquippedRightHand();
			if (equipped && equipped->IsWeapon()) {
				switch (equipped->As<RE::TESObjectWEAP>()->GetWeaponType()) {
				case RE::WEAPON_TYPE::kOneHandSword:
					distance = 310.0f;
					break;
				case RE::WEAPON_TYPE::kOneHandAxe:
					distance = 305.0f;
					break;
				case RE::WEAPON_TYPE::kOneHandMace:
					distance = 300.0f;
					break;
				case RE::WEAPON_TYPE::kOneHandDagger:
					distance = 250.0f;
					break;
				case RE::WEAPON_TYPE::kTwoHandAxe:
					distance = 350.0f;
					break;
				case RE::WEAPON_TYPE::kTwoHandSword:
					distance = 370.0;
					break;
				case RE::WEAPON_TYPE::kHandToHandMelee:
					if (!Utils::Actor::isHumanoid(actor)) {
						distance = 350.0f;
					} else {
						distance = 150.0f;
					}
					break;
				case RE::WEAPON_TYPE::kBow:
					distance = 1500.0;
					break;
				case RE::WEAPON_TYPE::kCrossbow:
					distance = 2100.0;
					break;
				case RE::WEAPON_TYPE::kStaff:
					distance = 320.0;
					break;
				default:
					distance = 150.0f;
					break;
				}

			} else if (equipped && equipped->IsArmor()) {
				distance = 250.0f;

			} else {
				if (!Utils::Actor::isHumanoid(actor)) {
					distance = 350.0f;
				} else {
					distance = 150.0f;
				}
			}
		} else {
			if (!Utils::Actor::isHumanoid(actor)) {
				distance = 350.0f;
			} else {
				distance = 150.0f;
			}
		}
		
	}else {
		if (!Utils::Actor::isHumanoid(actor)) {
			distance = 350.0f;
		} else {
			distance = 200.0f;
		}
	}

	return distance;
}

float dodge::Get_ReactiveDodge_Reach(RE::Actor* actor)
{
	float reach = Actor_GetReach(actor);
	return reach;
}

bool dodge::GetAttackSpell(RE::Actor* actor, bool lefthand) {

	bool result = false;
	auto limbospell = actor->GetActorRuntimeData().currentProcess;
	static auto fireKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFire");
	static auto frostKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFrost");
	static auto ShockKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageShock");

	if (limbospell) {
		if (lefthand){
			auto eSpell = limbospell->GetEquippedLeftHand();
			if (eSpell && eSpell->Is(RE::FormType::Spell)) {
				auto rSpell = eSpell->As<RE::SpellItem>();
				if (!(rSpell->GetSpellType() == RE::MagicSystem::SpellType::kPower || rSpell->GetSpellType() == RE::MagicSystem::SpellType::kLesserPower)){
					auto Effect_List = rSpell->effects;
					for (auto Effect : Effect_List) {
						if (Effect && Effect->baseEffect) {
							if (Effect->baseEffect->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kHostile)) {
								result = true;
								break;
							} else if (Effect->baseEffect->HasKeyword(fireKeyword) || Effect->baseEffect->HasKeyword(frostKeyword) || Effect->baseEffect->HasKeyword(ShockKeyword)) {
								result = true;
								break;
							}
						}
						continue;
					}
				}	
			}
		}else {
			auto eSpell = limbospell->GetEquippedRightHand();
			if (eSpell && eSpell->Is(RE::FormType::Spell)) {
				auto rSpell = eSpell->As<RE::SpellItem>();
				if (!(rSpell->GetSpellType() == RE::MagicSystem::SpellType::kPower || rSpell->GetSpellType() == RE::MagicSystem::SpellType::kLesserPower)) {
					auto Effect_List = rSpell->effects;
					for (auto Effect : Effect_List) {
						if (Effect && Effect->baseEffect) {
							if (Effect->baseEffect->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kHostile)) {
								result = true;
								break;
							} else if (Effect->baseEffect->HasKeyword(fireKeyword) || Effect->baseEffect->HasKeyword(frostKeyword) || Effect->baseEffect->HasKeyword(ShockKeyword)) {
								result = true;
								break;
							}
						}
						continue;
					}
				}
			}
		}
	}
	return result;
}

bool dodge::GetAttackSpell_Alt(RE::SpellItem* a_spell)
{
	bool result = false;
	static auto fireKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFire");
	static auto frostKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFrost");
	static auto ShockKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageShock");
	try
	{
		auto Effect_List = a_spell->effects;
		for (auto Effect : Effect_List) {
			if (Effect && Effect->baseEffect) {
				if (Effect->baseEffect->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kHostile)) {
					result = true;
					break;
				} else if (Effect->baseEffect->HasKeyword(fireKeyword) || Effect->baseEffect->HasKeyword(frostKeyword) || Effect->baseEffect->HasKeyword(ShockKeyword)) {
					result = true;
					break;
				}
			}
			continue;
		}
	}
	catch(...)
	{
		return result;
	}
	return result;
}

bool dodge::GetEquippedShout(RE::Actor* actor){
	bool result = false;
	auto limboshout = actor->GetActorRuntimeData().selectedPower;
	auto currentVar = actor->GetActorRuntimeData().currentProcess->high->currentShoutVariation;
	static auto fireKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFire");
	static auto frostKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageFrost");
	static auto ShockKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("MagicDamageShock");

	if (limboshout && limboshout->Is(RE::FormType::Shout) && currentVar){
		try
		{
			if (limboshout->As<RE::TESShout>()->variations && limboshout->As<RE::TESShout>()->variations[currentVar].spell) {
				auto eSpell = limboshout->As<RE::TESShout>()->variations[currentVar].spell;
				if (!(eSpell->GetSpellType() == RE::MagicSystem::SpellType::kPower || eSpell->GetSpellType() == RE::MagicSystem::SpellType::kLesserPower)){
					auto Effect_List = eSpell->effects;
					for (auto Effect : Effect_List) {
						if (Effect && Effect->baseEffect) {
							if (Effect->baseEffect->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kHostile)) {
								result = true;
								break;
							} else if (Effect->baseEffect->HasKeyword(fireKeyword) || Effect->baseEffect->HasKeyword(frostKeyword) || Effect->baseEffect->HasKeyword(ShockKeyword)) {
								result = true;
								break;
							}
						}
						continue;
					}
				}
			}
		}
		catch(...)
		{
			return result;
		}
		
	}
	return result;
}

float dodge::GetShoutRange_Reaction(RE::Actor* actor, float distance){
	float result = 0;
	auto limboshout = actor->GetActorRuntimeData().selectedPower;
	auto currentVar = actor->GetActorRuntimeData().currentProcess->high->currentShoutVariation;

	if (limboshout && limboshout->Is(RE::FormType::Shout) && currentVar){
		if (limboshout->As<RE::TESShout>()->variations && limboshout->As<RE::TESShout>()->variations[currentVar].spell) {
			auto eSpell = limboshout->As<RE::TESShout>()->variations[currentVar].spell;
			if (eSpell->GetDelivery() == RE::MagicSystem::Delivery::kAimed) {
				auto Effect_List = eSpell->effects;
				for (auto Effect : Effect_List) {
					if (Effect && Effect->baseEffect) {
						if (Effect->baseEffect->data.projectileBase) {
							float speed = Effect->baseEffect->data.projectileBase->data.speed;
							if (speed && speed != 0.0) {
								result = distance / speed;
								break;
							}
						}
					}
					continue;
				}
			}
		}
	}
	return result;
}

float dodge::GetSpellRange_Reaction(RE::Actor* actor, float distance, bool lefthand){
	float result = 0;
	auto limbospell = actor->GetActorRuntimeData().currentProcess;

	if (limbospell){
		if (lefthand) {
			auto eSpell = limbospell->GetEquippedLeftHand();
			if (eSpell && eSpell->Is(RE::FormType::Spell)){
				if (eSpell->As<RE::SpellItem>()->GetDelivery() == RE::MagicSystem::Delivery::kAimed) {
					auto Effect_List = eSpell->As<RE::SpellItem>()->effects;
					for (auto Effect : Effect_List) {
						if (Effect && Effect->baseEffect) {
							if (Effect->baseEffect->data.projectileBase) {
								float speed = Effect->baseEffect->data.projectileBase->data.speed;
								if (speed && speed != 0.0) {
									result = distance / speed;
									break;
								}
							}
						}
						continue;
					}
				}
			}
		} else {
			auto eSpell = limbospell->GetEquippedRightHand();
			if (eSpell && eSpell->Is(RE::FormType::Spell)) {
				if (eSpell->As<RE::SpellItem>()->GetDelivery() == RE::MagicSystem::Delivery::kAimed) {
					auto Effect_List = eSpell->As<RE::SpellItem>()->effects;
					for (auto Effect : Effect_List) {
						if (Effect && Effect->baseEffect) {
							if (Effect->baseEffect->data.projectileBase) {
								float speed = Effect->baseEffect->data.projectileBase->data.speed;
								if (speed && speed != 0.0) {
									result = distance / speed;
									break;
								}
							}
						}
						continue;
					}
				}
			}
		}
	}
	return result;
}


void dodge::Set_iFrames(RE::Actor* actor){
	actor->SetGraphVariableBool("bIframeActive", true);
	actor->SetGraphVariableBool("bInIframe", true);
}

void dodge::Reset_iFrames(RE::Actor* actor){
	actor->SetGraphVariableBool("bIframeActive", false);
	actor->SetGraphVariableBool("bInIframe", false);
}

void dodge::send_UNDdodge__event(RE::Actor* a_attacker)
{
	SKSE::ModCallbackEvent modEvent{
		RE::BSFixedString("UND_DodgeEvent"),
		RE::BSFixedString(),
		0.0f,
		a_attacker
	};

	SKSE::GetModCallbackEventSource()->SendEvent(&modEvent);
	logger::info("Sent melee parry event");
}

bool dodge::getrace_VLserana(RE::Actor* a_actor)
{
	bool result = false;
	const auto race = a_actor->GetRace();
	const auto raceEDID = race->formEditorID;
	if (raceEDID == "DLC1VampireBeastRace") {
		if (a_actor->HasKeywordString("VLS_Serana_Key") || a_actor->HasKeywordString("VLS_Valerica_Key")) {
			result = true;
		}
	}
	return result;
}

/*Trigger reactive AI surrounding the attacker.*/
void dodge::react_to_melee(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_reactive);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
						break;
					}
				}
				continue;
			}
		}
	}
}

void dodge::react_to_melee_power(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->Powerattack_attempt_dodge(refr, &dodge_directions_tk_reactive);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
						break;
					}
				}
				continue;
			}
		}
	}
}

void dodge::react_to_melee_normal(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->NormalAttack_attempt_dodge(refr, &dodge_directions_tk_reactive);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
						break;
					}
				}
				continue;
			}
		}
	}
}

void dodge::react_to_bash(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->Bash_attempt_dodge(refr, &dodge_directions_tk_reactive);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
						break;
					}
				}
				continue;
			}
		}
	}
}

void dodge::react_to_bash_sprint(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->BashSprint_attempt_dodge(refr, &dodge_directions_tk_reactive);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_all);
						break;
					}
				}
				continue;
			}
		}
	}
}

void dodge::react_to_ranged(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					bool hasLOS = false;
					if (refr->HasLineOfSight(a_attacker, hasLOS) && !hasLOS) {
						continue;
					}

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 7.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}
					int  bHeavyarmour = 0;
					auto Body = refr->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody);
					auto Helm = refr->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHair);
					auto Shield = refr->GetEquippedObject(true);
					if (Body) {
						switch (Body->GetArmorType()) {
						case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
							bHeavyarmour += 1;
							break;
						case RE::BIPED_MODEL::ArmorType::kLightArmor:
							bHeavyarmour += 0;
							break;
						case RE::BIPED_MODEL::ArmorType::kClothing:
							bHeavyarmour += 0;
							break;
						}
					} else {
						bHeavyarmour += 0;
					}

					if (Helm) {
						switch (Helm->GetArmorType()) {
						case RE::BIPED_MODEL::ArmorType::kHeavyArmor:
							bHeavyarmour += 1;
							break;
						case RE::BIPED_MODEL::ArmorType::kLightArmor:
							bHeavyarmour += 0;
							break;
						case RE::BIPED_MODEL::ArmorType::kClothing:
							bHeavyarmour += 0;
							break;
						}
					} else {
						bHeavyarmour += 0;
					}

					if (Shield && Shield->IsArmor()) {
						bHeavyarmour += 1;
					} else {
						bHeavyarmour += 0;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						if (bHeavyarmour >= 2 && (refr->GetEquippedObject(false)->As<RE::TESObjectWEAP>()->IsMelee())) {
							dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_ranged);
						} else {
							dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_horizontal);
						}
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
						break;
					}
				}
				continue;
			}
		}
	}
}

void dodge::react_to_shouts_spells(RE::Actor* a_attacker, float attack_range)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					bool hasLOS = false;
					if (refr->HasLineOfSight(a_attacker, hasLOS) && !hasLOS) {
						continue;
					}

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->Shouts_Spells_attempt_dodge(refr, &dodge_directions_tk_reactive);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
						break;
					}

					// float check = dodge::GetSingleton()->GetShoutRange_Reaction(refr, refr->GetPosition().GetDistance(a_attacker->GetPosition()));
					// if (check > 0.5f) {
					// 	auto check_int = static_cast<int>(check * 1000.0f);
					// 	for (start = Clock::now(), now = start; now < start + std::chrono::milliseconds{ check_int }; now = Clock::now()) {
					// 	}
					// 	switch (settings::iDodgeAI_Framework) {
					// 	case 0:
					// 		dodge::GetSingleton()->Shouts_Spells_attempt_dodge(refr, &dodge_directions_tk_reactive);
					// 		break;
					// 	case 1:
					// 		dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
					// 		break;
					// 	}
					// } else {
					// 	switch (settings::iDodgeAI_Framework) {
					// 	case 0:
					// 		dodge::GetSingleton()->Shouts_Spells_attempt_dodge(refr, &dodge_directions_tk_reactive);
					// 		break;
					// 	case 1:
					// 		dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
					// 		break;
					// 	}
					// }
				}
				continue;
			}
		}
	}
}

void dodge::react_to_shouts_spells_fast(RE::Actor* a_attacker, float attack_range, bool lefthand)
{
	if (!settings::bDodgeAI_Reactive_enable) {
		return;
	}
	auto combatGroup = a_attacker->GetCombatGroup();
	if (combatGroup) {
		for (auto it = combatGroup->targets.begin(); it != combatGroup->targets.end(); ++it) {
			if (it->targetHandle && it->targetHandle.get().get()) {
				RE::Actor* refr = it->targetHandle.get().get();
				if (refr->GetPosition().GetDistance(a_attacker->GetPosition()) <= attack_range) {
					
					if (refr->IsPlayerRef() || refr->IsDead() || !refr->IsInCombat()) {
						continue;
					}
					if (!Utils::Actor::isHumanoid(refr)) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (!(refr->HasKeywordString("ActorTypeNPC") || refr->HasKeywordString("DLC2ActorTypeMiraak"))) {
						if(!(refr->HasKeywordString("VLS_Serana_Key") || refr->HasKeywordString("VLS_Valerica_Key"))){
                            continue;
						}
					}
					if (refr->HasKeywordString("UND_ExcludeDodge")) {
						continue;
					}
					auto CTarget = refr->GetActorRuntimeData().currentCombatTarget.get().get();
					if (!CTarget) {
						continue;
					}
					// if (!ValhallaUtils::is_adversary(refr, a_attacker)) {
					// 	continue;
					// }

					bool hasLOS = false;
					if (refr->HasLineOfSight(a_attacker, hasLOS) && !hasLOS) {
						continue;
					}

					RE::BGSAttackData* attackdata = Utils::get_attackData(a_attacker);
					auto angle = Utils::get_angle_he_me(refr, a_attacker, attackdata);

					float attackAngle = attackdata ? attackdata->data.strikeAngle : 35.0f;

					if (abs(angle) > attackAngle) {
						continue;
					}

					switch (settings::iDodgeAI_Framework) {
					case 0:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_horizontal);
						break;
					case 1:
						dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
						break;
					}

					// if (lefthand){
					// 	float check = dodge::GetSingleton()->GetSpellRange_Reaction(refr, refr->GetPosition().GetDistance(a_attacker->GetPosition()), true);
					// 	if (check > 0.5f){
					// 		auto check_int = static_cast<int>(check * 1000.0f);
					// 		for (start = Clock::now(), now = start; now < start + std::chrono::milliseconds{ check_int }; now = Clock::now()) {
					// 		}
					// 		switch (settings::iDodgeAI_Framework) {
					// 		case 0:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_horizontal);
					// 			break;
					// 		case 1:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
					// 			break;
					// 		}
					// 	} else {
					// 		switch (settings::iDodgeAI_Framework) {
					// 		case 0:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_horizontal);
					// 			break;
					// 		case 1:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
					// 			break;
					// 		}
					// 	}
					// }else {
					// 	float check = dodge::GetSingleton()->GetSpellRange_Reaction(refr, refr->GetPosition().GetDistance(a_attacker->GetPosition()));
					// 	if (check > 0.5f) {
					// 		auto check_int = static_cast<int>(check * 1000.0f);
					// 		for (start = Clock::now(), now = start; now < start + std::chrono::milliseconds{ check_int }; now = Clock::now()) {
					// 		}
					// 		switch (settings::iDodgeAI_Framework) {
					// 		case 0:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_horizontal);
					// 			break;
					// 		case 1:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
					// 			break;
					// 		}
					// 	}else{
					// 		switch (settings::iDodgeAI_Framework) {
					// 		case 0:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_tk_horizontal);
					// 			break;
					// 		case 1:
					// 			dodge::GetSingleton()->attempt_dodge(refr, &dodge_directions_dmco_reactive);
					// 			break;
					// 		}
					// 	}
					// }
				}
				continue;
			}
		}
	}
}

//void dodge::set_dodge_phase(RE::Actor* a_dodger, bool a_isDodging)
//{
//	auto handle = a_dodger->GetHandle();
//	if (handle) {
//		writeLock l (dodging_actors_lock);
//		if (a_isDodging) {
//			if (!dodging_actors.contains(handle)) {
//				dodging_actors.insert(handle);
//			}
//		} else {
//			dodging_actors.erase(handle);
//		}
//	}
//}
//
//bool dodge::get_is_dodging(RE::Actor* a_actor)
//{
//	auto handle = a_actor->GetHandle();
//	if (handle) {
//		readLock l(dodging_actors_lock);
//		return dodging_actors.contains(handle);
//	}
//	return false;
//	
//}


/*Check if the actor is able to dodge.*/
bool dodge::able_dodge(RE::Actor* a_actor)
{
	auto attackState = a_actor->AsActorState()->GetAttackState();
	auto CombatTarget = a_actor->GetActorRuntimeData().currentCombatTarget.get().get();
	auto ATMagicTarget = a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kParalysis);
	auto magicTarget = a_actor->AsMagicTarget();
	bool IsShouting = false;
	auto DS = dodge::GetSingleton();
	const float SideStep_staminacost = DS->get_stamina_basecost(a_actor, DS->Staminaa);
	bool bUND_InCombatFoundEnemy = false;


	if (settings::bZUPA_mod_Check) {
		const auto magicEffect = RE::TESForm::LookupByEditorID("zxlice_cooldownEffect")->As<RE::EffectSetting>();

		if (!a_actor->IsInKillMove() && (a_actor->GetGraphVariableBool("bUND_InCombatFoundEnemy", bUND_InCombatFoundEnemy) && bUND_InCombatFoundEnemy) && (a_actor->GetGraphVariableBool("IsShouting", IsShouting) && !IsShouting) && !CombatTarget->AsActorState()->IsBleedingOut() && ATMagicTarget == 0.0 && !magicTarget->HasEffectWithArchetype(RE::EffectArchetypes::ArchetypeID::kDemoralize)
		&& (getrace_VLserana(a_actor)? a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka) >= 25 : a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= SideStep_staminacost) 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough || attackState == RE::ATTACK_STATE_ENUM::kBash 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough) && !magicTarget->HasMagicEffect(magicEffect)) {
			return true;
		}
	} else if (settings::bUAPNG_mod_Check){
		bool IUBusy = false;
		if (!a_actor->IsInKillMove() && (a_actor->GetGraphVariableBool("bUND_InCombatFoundEnemy", bUND_InCombatFoundEnemy) && bUND_InCombatFoundEnemy) && (a_actor->GetGraphVariableBool("IsShouting", IsShouting) && !IsShouting) && !CombatTarget->AsActorState()->IsBleedingOut() && ATMagicTarget == 0.0 && !magicTarget->HasEffectWithArchetype(RE::EffectArchetypes::ArchetypeID::kDemoralize)
		&& (a_actor->GetGraphVariableBool("IUBusy", IUBusy) && !IUBusy) && (getrace_VLserana(a_actor)? a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka) >= 25 : a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= SideStep_staminacost) 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough || attackState == RE::ATTACK_STATE_ENUM::kBash 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
			return true;
		}

	} else{
		if (!a_actor->IsInKillMove() && (a_actor->GetGraphVariableBool("bUND_InCombatFoundEnemy", bUND_InCombatFoundEnemy) && bUND_InCombatFoundEnemy) && (a_actor->GetGraphVariableBool("IsShouting", IsShouting) && !IsShouting) && !CombatTarget->AsActorState()->IsBleedingOut() && ATMagicTarget == 0.0 && !magicTarget->HasEffectWithArchetype(RE::EffectArchetypes::ArchetypeID::kDemoralize)
		&& (getrace_VLserana(a_actor)? a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMagicka) >= 25 : a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= SideStep_staminacost) 
		&& !(attackState == RE::ATTACK_STATE_ENUM::kSwing || attackState == RE::ATTACK_STATE_ENUM::kHit  || attackState == RE::ATTACK_STATE_ENUM::kFollowThrough || attackState == RE::ATTACK_STATE_ENUM::kBash 
		|| attackState == RE::ATTACK_STATE_ENUM::kBowDrawn || attackState == RE::ATTACK_STATE_ENUM::kBowReleasing || attackState == RE::ATTACK_STATE_ENUM::kBowFollowThrough)) {
			return true;
		}
	}
	return false;
}

/*Attempt to dodge an incoming threat, choosing one of the directions from A_DIRECTIONS.*/
void dodge::attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
	
    auto DS = dodge::GetSingleton();
	const float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor, DS->Armourr, DS->Protagnist_Reflexess, DS->CStylee);

	

	std::mt19937 gen(rd());
	
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor)) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				
				do_dodge(a_actor, direction);
				logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::Powerattack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
    auto DS = dodge::GetSingleton();
	const float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor, DS->Armourr, DS->Protagnist_Reflexess, DS->CStylee);

	

	std::mt19937 gen(rd());
	
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor)) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				
				do_dodge(a_actor, direction);
				logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::NormalAttack_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
    auto DS = dodge::GetSingleton();
	const float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor, DS->Armourr, DS->Protagnist_Reflexess, DS->CStylee);

	

	std::mt19937 gen(rd());
	
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor)) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				
				do_dodge(a_actor, direction);
				logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::Shouts_Spells_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{

	auto DS = dodge::GetSingleton();
	const float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor, DS->Armourr, DS->Protagnist_Reflexess, DS->CStylee);

	

	std::mt19937 gen(rd());
	
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor)) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				
				do_dodge(a_actor, direction);
				logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::Bash_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{
    auto DS = dodge::GetSingleton();
	const float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor, DS->Armourr, DS->Protagnist_Reflexess, DS->CStylee);

	

	std::mt19937 gen(rd());
	
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor)) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				
				do_dodge(a_actor, direction);
				logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);
			}
			return;
		} else {
			return;
		}
	}
}

void dodge::BashSprint_attempt_dodge(RE::Actor* a_actor, const dodge_dir_set* a_directions, bool a_forceDodge)
{

    auto DS = dodge::GetSingleton();
	const float dodge_chance = a_forceDodge ? 1.0f : get_dodge_chance(a_actor, DS->Armourr, DS->Protagnist_Reflexess, DS->CStylee);

	

	std::mt19937 gen(rd());
	
	if (dodge::GetSingleton()->GenerateRandomFloat(0.0, 1.0) > dodge_chance) {
		return;
	}

	/* Make a copy and shuffle directions. */
	dodge_dir_set directions_shuffled = *a_directions;
	std::shuffle(directions_shuffled.begin(), directions_shuffled.end(), gen);

	for (dodge_direction direction : directions_shuffled) {
		RE::NiPoint3 dodge_dest = Utils::get_abs_pos(a_actor, get_dodge_vector(direction));
		if (can_goto(a_actor, dodge_dest) && able_dodge(a_actor)) {
			bool bIsDodging = false;
			if (a_actor->GetGraphVariableBool("bIsDodging", bIsDodging) && !bIsDodging) {
				
				do_dodge(a_actor, direction);
				logger::info("Protagnist {} ReflexScore {}"sv, a_actor->GetName(), dodge_chance);
			}
			return;
		} else {
			return;
		}
	}
}


#define MAX_DIST_DIFFERENCE 50
bool dodge::can_goto(RE::Actor* a_actor, RE::NiPoint3 a_dest)
{
	bool canNavigate = false;
	bool noObstacle = true;
	RE::NiPoint3 raycast_dest = a_dest;
	RE::NiPoint3 nav_dest = a_dest;

	bool gotoNavdest = false;
	/*float expect_dist = a_actor->GetPosition().GetDistance(a_dest);*/
	
	// /*Use Skyrim's internal check*/
	// if (a_actor->UpdateNavPos(a_actor->GetPosition(), nav_dest, 4.0f, a_actor->GetBoundRadius()) 
	// 	&& abs(nav_dest.GetDistance(a_actor->GetPosition()) - expect_dist) < MAX_DIST_DIFFERENCE) {
	// 	RE::NiPoint3 nav_dest_raycast = nav_dest;                   // make a copy of nav_dest to use for raycast
	// 	if (DtryUtils::rayCast::object_exists(nav_dest_raycast)) {//check if the actor can stand on the nav dest
	// 		gotoNavdest = true;
	// 		canNavigate = true;
	// 	}
	// }
	
	/*Use our own pathing check, if skyrim's check fails.*/
	if (!gotoNavdest) {
		canNavigate = DtryUtils::rayCast::object_exists(raycast_dest, 75.f);//bigger range to account for slopes
	}
	
	if (canNavigate) {
		RE::NiPoint3 dest = gotoNavdest ? nav_dest : raycast_dest;
		//if (settings::bDodgeAI_DebugDraw_Enable) {
		//	if (API::TrueHUD_API_acquired) {
		//		API::_TrueHud_API->DrawLine(a_actor->GetPosition(), dest, 1.f, 0xff00ff);  //green line
		//	}
		//}
		
		/*Cast 3 rays from the actor, parallel to the dodging path to check for any obstacles.*/
		float obstacleDist = 0; /*Distance to the obstacle, if any*/
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.25f, &obstacleDist) == nullptr || obstacleDist >= settings::fDodgeAI_DodgeDist_Permissible;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.5f, &obstacleDist) == nullptr || obstacleDist >= settings::fDodgeAI_DodgeDist_Permissible;
		dest.z += a_actor->GetHeight() * 1 / 4;
		noObstacle &= DtryUtils::rayCast::cast_ray(a_actor, dest, 0.75f, &obstacleDist) == nullptr || obstacleDist >= settings::fDodgeAI_DodgeDist_Permissible;
	}
	

	return canNavigate && noObstacle;
	
}


/*Get the direction the actor should dodge in.*/
//dodge_direction dodge::get_dodge_direction(RE::Actor* a_actor, RE::Actor* a_attacker)
//{
//	return dodge_direction::kForward; /*defaults to backward dodging for now*/
//}

int dodge::GenerateRandomInt(int value_a, int value_b) {

	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> dist(value_a, value_b);
	return dist(generator);
}

float dodge::GenerateRandomFloat(float value_a, float value_b)
{
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> dist(value_a, value_b);
	return dist(generator);
}

static const char* GVI_dodge_dir = "Dodge_Direction";
// static const char* AE_dodge = "Dodge";
void dmco_dodge(RE::Actor* a_actor, dodge_direction a_direction, const char* a_event) {
	auto task = SKSE::GetTaskInterface();
	if (!task) {
		return;
	}
	task->AddTask([a_actor, a_direction, a_event]() {
		a_actor->SetGraphVariableInt(GVI_dodge_dir, a_direction);
		interruptattack(a_actor);
		a_actor->NotifyAnimationGraph(a_event);
	});
}

void dodge::TRKE_dodge(RE::Actor* actor, const char* a_event, bool backingoff)
{
	auto DS = dodge::GetSingleton();
	const float DodgeRoll_staminacost = DS->get_stamina_basecost(actor, DS->Staminaa, true);
	actor->NotifyAnimationGraph("InterruptCast");
	actor->InterruptCast(false);

	if (backingoff) {
		actor->SetGraphVariableInt("iStep", 2);
		actor->SetGraphVariableBool("bUND_IsDodgeRoll", false);
		actor->NotifyAnimationGraph(a_event);
		return;
	}

	if (settings::bHasSilentRollperk_enable == 1) {
		auto data = RE::TESDataHandler::GetSingleton();
		auto bSilentRoll = actor->HasPerk(data->LookupForm<RE::BGSPerk>(0x105F23, "Skyrim.esm"));
		if (dodge::GetSingleton()->GenerateRandomInt(0, 10) <= settings::iDodgeRoll_ActorScaled_Chance && bSilentRoll && actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= DodgeRoll_staminacost) {
			actor->SetGraphVariableInt("iStep", 0);
			actor->SetGraphVariableBool("bUND_IsDodgeRoll", true);
		} else {
			actor->SetGraphVariableInt("iStep", 2);
			actor->SetGraphVariableBool("bUND_IsDodgeRoll", false);
		}
		actor->NotifyAnimationGraph(a_event);
		return;

	} else {
		if (dodge::GetSingleton()->GenerateRandomInt(0, 10) <= settings::iDodgeRoll_ActorScaled_Chance && actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) >= DodgeRoll_staminacost) {
			actor->SetGraphVariableInt("iStep", 0);
			actor->SetGraphVariableBool("bUND_IsDodgeRoll", true);	
		} else {
			actor->SetGraphVariableInt("iStep", 2);
			actor->SetGraphVariableBool("bUND_IsDodgeRoll", false);
		}
		actor->NotifyAnimationGraph(a_event);

	}
}

void dodge::do_dodge(RE::Actor* a_actor, dodge_direction a_direction)
{
	switch (a_direction) {
	case dodge_direction::kForward:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeForward");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kBackward:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeBack");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kLeft:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeLeft");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRight:
		switch (settings::iDodgeAI_Framework) {
		case 0:

			TRKE_dodge(a_actor, "TKDodgeRight");
			break;
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	/*Only possible for DMCO*/
	case dodge_direction::kLeftBackward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kLeftForward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRightBackward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	case dodge_direction::kRightForward:
		switch (settings::iDodgeAI_Framework) {
		case 1:
			dmco_dodge(a_actor, a_direction, "Dodge");
			break;
		}
		break;
	}
}
/*Get relative dodge vector*/
RE::NiPoint3 dodge::get_dodge_vector(dodge_direction a_direction)
{
	RE::NiPoint3 ret;
	ret.z = 0;
	switch (a_direction) {
	case kForward:
		ret.x = 0;
		ret.y = settings::fDodgeAI_DodgeDist;
		break;
	case kBackward:
		ret.x = 0;
		ret.y = -settings::fDodgeAI_DodgeDist;
		break;
	case kLeft:
		ret.x = -settings::fDodgeAI_DodgeDist;
		ret.y = 0;
		break;
	case kRight:
		ret.x = settings::fDodgeAI_DodgeDist;
		ret.y = 0;
		break;
	case kLeftBackward:
		ret.x = -settings::fDodgeAI_DodgeDist2;
		ret.y = -settings::fDodgeAI_DodgeDist2;
		break;
	case kLeftForward:
		ret.x = -settings::fDodgeAI_DodgeDist2;
		ret.y = settings::fDodgeAI_DodgeDist2;
		break;
	case kRightBackward:
		ret.x = settings::fDodgeAI_DodgeDist2;
		ret.y = -settings::fDodgeAI_DodgeDist2;
		break;
	case kRightForward:
		ret.x = settings::fDodgeAI_DodgeDist2;
		ret.y = settings::fDodgeAI_DodgeDist2;
		break;
	}
	
	return ret;
}

void dodge::do_dodge_VLSerana(RE::Actor* a_actor, dodge_direction a_direction)
{
	auto HdSingle = RE::TESDataHandler::GetSingleton();
	RE::TESGlobal* Xcoord = nullptr;
	RE::TESGlobal* Ycoord = nullptr;

	if (a_actor->HasKeywordString("VLS_Serana_Key")) {
		RE::TESGlobal* Xcoord = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x800, "No Spell Shout FF.esp"));
		RE::TESGlobal* Ycoord = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x800, "No Spell Shout FF.esp"));
		if (Xcoord && Ycoord){
			switch (a_direction) {
			case kForward:
			    Xcoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				Ycoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				break;
			case kBackward:
				Xcoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				Ycoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				break;
			case kLeft:
				Xcoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				Ycoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				break;
			case kRight:
				Xcoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				Ycoord->value = GetSingleton()->GenerateRandomFloat(1.0f, 2.0f);
				break;
			}
		}

	} else if (a_actor->HasKeywordString("VLS_Valerica_Key")) {
		RE::TESGlobal* Xcoord = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x800, "No Spell Shout FF.esp"));
		RE::TESGlobal* Ycoord = skyrim_cast<RE::TESGlobal*>(HdSingle->LookupForm(0x800, "No Spell Shout FF.esp"));
		if (Xcoord && Ycoord) {
			switch (a_direction) {
			case kForward:

				break;
			case kBackward:

				break;
			case kLeft:

				break;
			case kRight:

				break;
			}
		}
	}
}