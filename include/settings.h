#pragma once
#define SQRT2 1.4142135624f
#include <SimpleIni.h>
#include "Utils.h"
#include "dodge.h"

using EventResult = RE::BSEventNotifyControl;

class settings
{
public:
	static settings* GetSingleton()
	{
		static settings singleton;
		return &singleton;
	}

	class update_handler : public RE::BSTEventSink<SKSE::ModCallbackEvent>
	{
	public:
		virtual EventResult ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>* a_eventSource);

		static bool Register();
	};
	static void read(const wchar_t* a_path);
	static void init();
	static void setglobals();

	/*Dodging*/
	inline static bool bDodgeAI_Enable = true; /*Global switch for dodge AI*/
	inline static bool bTacticalDodgeAI_enable = true; /*Passive dodge AI that's built on top of vanilla's combat movement AI.*/
	inline static bool bDodgeAI_Reactive_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/

	// inline static bool bRecoilStunBreak_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/
	inline static bool biFrames_enable = true; /*Reactive dodge AI that dodges incoming attacks.*/
	inline static int bHasSilentRollperk_enable = 1; /*Reactive dodge AI that dodges incoming attacks.*/
	inline static bool bStaminaCost_enable = true;   /*Reactive dodge AI that dodges incoming attacks.*/
	inline static bool bStaminaCost_ArmourBasedSystem_enable = true;   /*Reactive dodge AI that dodges incoming attacks.*/
	inline static bool bZUPA_mod_Check = false;   /*Reactive dodge AI that dodges incoming attacks.*/
	inline static bool bUAPNG_mod_Check = false;   /*Reactive dodge AI that dodges incoming attacks.*/

	static inline int iDodgeRoll_ActorScaled_Chance = 2;           /*TKRE = 0; DMCO = 1*/
	static inline int iReactiveDodgeAI_enable = 1;

	static inline bool bDodgeAI_AttackCancel_enable = false; /*Cancel attack animations when dodging.*/
	static inline int iDodgeAI_Framework = 0;            /*TKRE = 0; DMCO = 1*/
	
	static inline float fDodgeAI_Chance_Mult = 1.0f; /*Chance multiplier for reactive dodge AI.*/
	static inline float fDodgeAI_Reactive_Dist = 350.0f;               /**/
	static inline float fDodgeAI_DodgeDist = 300.0f;          /*Distance of a single dodge, used for pathing.*/
	static inline float fDodgeAI_DodgeDist2 = fDodgeAI_DodgeDist / SQRT2; /* Cached coordinate distance.*/
	
	static inline float fDodgeAI_DodgeDist_Permissible = 200.0f; /*Acceptable distance of a single dodge. 
													  * Sometimes the dodger can get obstructed by a wall before completing the whole dodge while already making a big displacement.
													  * AI would take dodges that can make this displacement as a permissible(tho suboptimal) dodge and still make the dodge.*/
	static inline bool bDodgeAI_DebugDraw_Enable = false;


};
