#include "hooks.h"
#include "settings.h"
#include "ClibUtil/editorID.hpp"

namespace hooks
{

	RE::BSEventNotifyControl animEventHandler::HookedProcessEvent(RE::BSAnimationGraphEvent &a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *src)
	{
		FnProcessEvent fn = fnHash.at(*(uint64_t *)this);

		if (!a_event.holder){
			return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
		}

		//std::string eventTag = a_event->tag.c_str();
		RE::Actor* actor = const_cast<RE::TESObjectREFR*>(a_event.holder)->As<RE::Actor>();
		//bool bUseAltAtk = false;
		switch (hash(a_event.tag.c_str(), a_event.tag.size())) {
		case "TKDR_DodgeStart"_h:
		    if (!actor->IsPlayerRef()) {
				const auto StaminaCost = RE::TESForm::LookupByEditorID<RE::MagicItem>("StaminaCostSpell_UND");
				const auto caster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
				auto DS = dodge::GetSingleton();
				const float DodgeRoll_staminacost = DS->get_stamina_basecost(actor, DS->Staminaa, true);
				const float SideStep_staminacost = DS->get_stamina_basecost(actor, DS->Staminaa);
				bool bUND_IsDodgeRoll = false;
				if (actor->GetGraphVariableBool("bUND_IsDodgeRoll", bUND_IsDodgeRoll) && bUND_IsDodgeRoll) {
					if (settings::bStaminaCost_enable) {
						logger::info("Protagnist {} DodgeRollCost {}"sv, actor->GetName(), DodgeRoll_staminacost);
						caster->CastSpellImmediate(StaminaCost, true, actor, 1, false, -(DodgeRoll_staminacost), actor);
					}
					if (settings::biFrames_enable) {
						dodge::Set_iFrames(actor);
					}
					break;
				} else {
					if (settings::bStaminaCost_enable) {
						logger::info("Protagnist {} SideStepCost {}"sv, actor->GetName(), SideStep_staminacost);
						caster->CastSpellImmediate(StaminaCost, true, actor, 1, false, -(SideStep_staminacost), actor);
					}
					if (settings::biFrames_enable) {
						dodge::Set_iFrames(actor);
					}
				}
			}
			break;

		case "TKDR_DodgeEnd"_h:
			if (!actor->IsPlayerRef()) {
				if (settings::biFrames_enable) {
					dodge::Reset_iFrames(actor);
				}
			}
			break;
	
		case "preHitFrame"_h:
			if ((!Utils::Actor::isHumanoid(actor) || Utils::Actor::isPowerAttacking(actor)) 
			&& (!(actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash || actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kHit))) {
				
				dodge::GetSingleton()->react_to_melee(actor, dodge::GetSingleton()->Get_ReactiveDodge_Distance(actor));

			} else if ((!Utils::Actor::isPowerAttacking(actor) && actor->AsActorState()->GetAttackState() != RE::ATTACK_STATE_ENUM::kBash) 
			&& (actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kDraw || actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kSwing)) {
				
				dodge::GetSingleton()->react_to_melee(actor, dodge::GetSingleton()->Get_ReactiveDodge_Distance(actor));

			} else if (actor->AsActorState()->IsSprinting() && actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash) {
				bool bMaxsuWeaponParry_InWeaponParry = false;
				if ((actor)
						->GetGraphVariableBool("bMaxsuWeaponParry_InWeaponParry", bMaxsuWeaponParry_InWeaponParry) &&
					!bMaxsuWeaponParry_InWeaponParry) {
					dodge::GetSingleton()->react_to_bash_sprint(actor, dodge::GetSingleton()->Get_ReactiveDodge_Distance(actor));
				}
			} else if (!actor->AsActorState()->IsSprinting() && actor->AsActorState()->GetAttackState() == RE::ATTACK_STATE_ENUM::kBash && Utils::Actor::isPowerAttacking(actor)) {
				bool bMaxsuWeaponParry_InWeaponParry = false;
				if ((actor)
						->GetGraphVariableBool("bMaxsuWeaponParry_InWeaponParry", bMaxsuWeaponParry_InWeaponParry) &&
					!bMaxsuWeaponParry_InWeaponParry) {
					dodge::GetSingleton()->react_to_bash(actor, dodge::GetSingleton()->Get_ReactiveDodge_Distance(actor));
				}
			}
			break;

		// case "Voice_SpellFire_Event"_h:
		// 	if (actor->GetGraphVariableBool("bUseAltAtk", bUseAltAtk) && !bUseAltAtk) {
		// 		if (GetEquippedShouts(actor) && dodge::GetSingleton()->GetEquippedShout(actor)) {
		// 			dodge::GetSingleton()->react_to_shouts_spells(actor, 3000.0f);
		// 		}
		// 	}
		// 	//dodge::GetSingleton()->react_to_shouts_spells(actor, 3000.0f);
		// 	break;

		// case "MLh_SpellFire_Event"_h:
		// 	if (dodge::GetSingleton()->GetAttackSpell(actor, true)) {
		// 		dodge::GetSingleton()->react_to_shouts_spells_fast(actor, 3000.0f, true);
		// 	}
		// 	break;

		// case "MRh_SpellFire_Event"_h:
		// 	if (dodge::GetSingleton()->GetAttackSpell(actor)) {
		// 		dodge::GetSingleton()->react_to_shouts_spells_fast(actor, 3000.0f);
		// 	}
		// 	break;

		case "PowerAttack_Start_end"_h:
		case "NextAttackInitiate"_h:
			dodge::GetSingleton()->react_to_melee_normal(actor, dodge::GetSingleton()->Get_ReactiveDodge_Distance(actor));
			break;

		case "BowFullDrawn"_h:

			dodge::GetSingleton()->react_to_ranged(actor, dodge::GetSingleton()->Get_ReactiveDodge_Distance(actor));
			break;
		}

		return fn ? (this->*fn)(a_event, src) : RE::BSEventNotifyControl::kContinue;
	}

	std::unordered_map<uint64_t, animEventHandler::FnProcessEvent> animEventHandler::fnHash;

	bool GetEquippedShouts(RE::SpellItem* a_spell)
	{
		bool result = true;
		std::string Lsht = (clib_util::editorID::get_editorID(a_spell));
		switch (hash(Lsht.c_str(), Lsht.size())) {
		case "HoY_PullofNirn_Miraak"_h:
		case "VoiceSlowTime1"_h:
		case "VoiceSlowTime2"_h:
		case "VoiceSlowTime3"_h:
		case "Serio_EDR_GravityBlastSpellPAAR"_h:
		case "Serio_EDR_GravityBlastSpellODAH"_h:
		case "VoiceAlduinTimeStop"_h:
			result = false;
			break;
		default:
			break;
		}
		return result;
	}

	class OurEventSink : public RE::BSTEventSink<RE::TESCombatEvent>, public RE::BSTEventSink<RE::TESSpellCastEvent>
	{
		OurEventSink() = default;
		OurEventSink(const OurEventSink&) = delete;
		OurEventSink(OurEventSink&&) = delete;
		OurEventSink& operator=(const OurEventSink&) = delete;
		OurEventSink& operator=(OurEventSink&&) = delete;

	public:
		static OurEventSink* GetSingleton()
		{
			static OurEventSink singleton;
			return &singleton;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESCombatEvent* event, RE::BSTEventSource<RE::TESCombatEvent>*)
		{
			auto Protagonist = event->actor->As<RE::Actor>();

			if (!Protagonist){
				return RE::BSEventNotifyControl::kContinue;
			}

			if (Protagonist->IsPlayerRef() || !Utils::Actor::isHumanoid(Protagonist)) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (!(Protagonist->HasKeywordString("ActorTypeNPC") || Protagonist->HasKeywordString("DLC2ActorTypeMiraak"))) {
				return RE::BSEventNotifyControl::kContinue;
			}

			if (Protagonist->HasKeywordString("UND_ExcludeDodge")) {
				return RE::BSEventNotifyControl::kContinue;
			}

			auto getcombatstate = event->newState.get();

			if (getcombatstate == RE::ACTOR_COMBAT_STATE::kCombat) {
				Protagonist->SetGraphVariableBool("bUND_InCombatFoundEnemy", true);
			} else {
				Protagonist->SetGraphVariableBool("bUND_InCombatFoundEnemy", false);
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::TESSpellCastEvent* event, RE::BSTEventSource<RE::TESSpellCastEvent>*)
		{
			auto Protagonist = event->object->As<RE::Actor>();

			if (!Protagonist) {
				return RE::BSEventNotifyControl::kContinue;
			}

			auto eSpell = RE::TESForm::LookupByID(event->spell);

			if (eSpell && eSpell->Is(RE::FormType::Spell)) {
				auto rSpell = eSpell->As<RE::SpellItem>();
				switch (rSpell->GetSpellType()) {
				case RE::MagicSystem::SpellType::kVoicePower:
				case RE::MagicSystem::SpellType::kPower:
				case RE::MagicSystem::SpellType::kLesserPower:
					if (GetEquippedShouts(rSpell) && dodge::GetSingleton()->GetAttackSpell_Alt(rSpell)) {
						dodge::GetSingleton()->react_to_shouts_spells(Protagonist, 3000.0f);
					}
					break;
				case RE::MagicSystem::SpellType::kSpell:
				case RE::MagicSystem::SpellType::kScroll:
					if (dodge::GetSingleton()->GetAttackSpell_Alt(rSpell)) {
						dodge::GetSingleton()->react_to_shouts_spells(Protagonist, 3000.0f);
					}
					break;
				case RE::MagicSystem::SpellType::kStaffEnchantment:
					if (rSpell->GetDelivery() != RE::MagicSystem::Delivery::kTouch && dodge::GetSingleton()->GetAttackSpell_Alt(rSpell)) {
						dodge::GetSingleton()->react_to_shouts_spells(Protagonist, 3000.0f);
					}
					break;
				default:
					break;
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}
	};

	void IHooks::install()
	{
		auto eventSink = OurEventSink::GetSingleton();

		// ScriptSource
		auto* eventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		eventSourceHolder->AddEventSink<RE::TESCombatEvent>(eventSink);
		eventSourceHolder->AddEventSink<RE::TESSpellCastEvent>(eventSink);
	}

	ptr_CombatPath on_combatBehavior_backoff_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		if (!Utils::Actor::isHumanoid(a_actor) || a_actor->HasKeywordString("UND_ExcludeDodge")) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		if (!(a_actor->HasKeywordString("ActorTypeNPC") || a_actor->HasKeywordString("DLC2ActorTypeMiraak"))) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_back);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_back);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_circle_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		if (!Utils::Actor::isHumanoid(a_actor) || a_actor->HasKeywordString("UND_ExcludeDodge")) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		if (!(a_actor->HasKeywordString("ActorTypeNPC") || a_actor->HasKeywordString("DLC2ActorTypeMiraak"))) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_horizontal);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_horizontal);
			break;
		}
	
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_fallback_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		if (!Utils::Actor::isHumanoid(a_actor) || a_actor->HasKeywordString("UND_ExcludeDodge")) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		if (!(a_actor->HasKeywordString("ActorTypeNPC") || a_actor->HasKeywordString("DLC2ActorTypeMiraak"))) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_back);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_back);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}

	ptr_CombatPath on_combatBehavior_dodgethreat_createPath::create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind)
	{
		if (!Utils::Actor::isHumanoid(a_actor) || a_actor->HasKeywordString("UND_ExcludeDodge")) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		if (!(a_actor->HasKeywordString("ActorTypeNPC") || a_actor->HasKeywordString("DLC2ActorTypeMiraak"))) {
			return _create_path(a_actor, a_newPos, a3, speed_ind);
		}

		switch (settings::iDodgeAI_Framework) {
		case 0:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_tk_all);
			break;
		case 1:
			dodge::GetSingleton()->attempt_dodge(a_actor, &dodge_directions_dmco_all);
			break;
		}
		
		return _create_path(a_actor, a_newPos, a3, speed_ind);
	}
	


	//inline void offset_NPC_rotation(RE::Actor* a_actor, float& a_angle) 
	//{
	//	if (a_actor->IsPlayerRef()) {
	//		return;
	//	}
	//	float angleDelta = Utils::math::NormalRelativeAngle(a_angle - a_actor->data.angle.z);
	//
	//	/* Dodge commitment */
	//	if (settings::bDodgeAI_Enable && dodge::GetSingleton()->get_is_dodging(a_actor)) {  
	//		angleDelta = 0;
	//	}
	//	a_angle = a_actor->data.angle.z + angleDelta;
	//}

	
	/*void on_set_rotation::Actor_SetRotationX(RE::Actor* a_this, float a_angle)
	{
		offset_NPC_rotation(a_this, a_angle);
		_Actor_SetRotationX(a_this, a_angle);
	}

	void on_set_rotation::Actor_SetRotationZ(RE::Actor* a_this, float a_angle)
	{
		offset_NPC_rotation(a_this, a_angle);
		_Actor_SetRotationZ(a_this, a_angle);
	}*/

};
