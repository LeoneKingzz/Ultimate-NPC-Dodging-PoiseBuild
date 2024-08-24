#include "SKSE/Trampoline.h"
#pragma warning(disable: 4100)


namespace hooks
{

	using EventResult = RE::BSEventNotifyControl;
	
	static bool GetEquippedShouts(RE::Actor* actor);

	class IHooks
	{
	public:
		static void install();
	};

	class animEventHandler
	{
	private:
		template <class Ty>
		static Ty SafeWrite64Function(uintptr_t addr, Ty data)
		{
			DWORD oldProtect;
			void *_d[2];
			memcpy(_d, &data, sizeof(data));
			size_t len = sizeof(_d[0]);

			VirtualProtect((void *)addr, len, PAGE_EXECUTE_READWRITE, &oldProtect);
			Ty olddata;
			memset(&olddata, 0, sizeof(Ty));
			memcpy(&olddata, (void *)addr, len);
			memcpy((void *)addr, &_d[0], len);
			VirtualProtect((void *)addr, len, oldProtect, &oldProtect);
			return olddata;
		}

		typedef RE::BSEventNotifyControl (animEventHandler::*FnProcessEvent)(RE::BSAnimationGraphEvent &a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *dispatcher);

		RE::BSEventNotifyControl HookedProcessEvent(RE::BSAnimationGraphEvent &a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent> *src);

		static void HookSink(uintptr_t ptr)
		{
			FnProcessEvent fn = SafeWrite64Function(ptr + 0x8, &animEventHandler::HookedProcessEvent);
			fnHash.insert(std::pair<uint64_t, FnProcessEvent>(ptr, fn));
		}

	public:
		/*Hook anim event sink*/
		static void Register(bool player, bool NPC)
		{
			if (player)
			{
				logger::info("Sinking animation event hook for player");
				REL::Relocation<uintptr_t> pcPtr{RE::VTABLE_PlayerCharacter[2]};
				HookSink(pcPtr.address());
			}
			if (NPC)
			{
				logger::info("Sinking animation event hook for NPC");
				REL::Relocation<uintptr_t> npcPtr{RE::VTABLE_Character[2]};
				HookSink(npcPtr.address());
			}
			logger::info("Sinking complete.");
		}

		static void RegisterForPlayer()
		{
			Register(true, false);
		}

	protected:
		static std::unordered_map<uint64_t, FnProcessEvent> fnHash;
	};
	
	typedef uintptr_t ptr_CombatPath;
	class on_combatBehavior_backoff_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ REL::RelocationID(46724, 47920) }; //SE::sub_1407D8C90+2D0	call    Character__sub_14083BE20 AE:: sub_140804020+284	call    sub_14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + REL::Relocate(0x2D0, 0x284), create_path); // SE::
			logger::info("hook:on_combatBehavior_backoff_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
		
	};

	class on_combatBehavior_circle_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ REL::RelocationID(46720, 47916) };  //SE::sub_1407D8200+593	call    Character__sub_14083BE20 AE:: 140803580+5F0 CALL 14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + REL::Relocate(0x593, 0x5F0), create_path);
			logger::info("hook:on_combatBehaiovr_circle_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
	};
	
	class on_combatBehavior_fallback_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ REL::RelocationID(46712, 47908) };  //SE::sub_1407D73D0+299	call    Character__sub_14083BE20 AE::sub_140802680+257	call    sub_14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + REL::Relocate(0x299, 0x257), create_path);
			logger::info("hook:on_combatBehavior_fallback_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
	};

	class on_combatBehavior_dodgethreat_createPath
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<std::uintptr_t> on_combatBehavior_backoff{ REL::RelocationID(46590, 47802) };  //SE::sub_1407D1740+82E	call    Character__sub_14083BE20 AE::1407FC830 + 7A0 call 14086B7C0
			_create_path = trampoline.write_call<5>(on_combatBehavior_backoff.address() + REL::Relocate(0x82E, 0x7A0), create_path);
			logger::info("hook:on_combatBehavior_dodgethreat_createPath");
		}
		
	private:
		static ptr_CombatPath create_path(RE::Actor* a_actor, RE::NiPoint3* a_newPos, float a3, int speed_ind);
		static inline REL::Relocation<decltype(create_path)> _create_path;
	};
	



	class on_set_rotation
	{
	public:
		static void install()
		{
			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<uintptr_t> hook1{ REL::RelocationID(32042, 32796) };  // 4EC300, 504B30  // synchronized anims
			REL::Relocation<uintptr_t> hook2{ REL::RelocationID(36365, 37356) };  // 5D87F0, 5FD7E0

			_Actor_SetRotationX = trampoline.write_call<5>(hook1.address() + REL::Relocate(0x4DC, 0x667), Actor_SetRotationX);  // 4EC7DC
			_Actor_SetRotationZ = trampoline.write_call<5>(hook2.address() + REL::Relocate(0x9C7, 0xA87), Actor_SetRotationZ);  // 5D91B7
		}

	private:
		static void Actor_SetRotationX(RE::Actor* a_this, float a_angle);
		static void Actor_SetRotationZ(RE::Actor* a_this, float a_angle);
		static inline REL::Relocation<decltype(Actor_SetRotationX)> _Actor_SetRotationX;
		static inline REL::Relocation<decltype(Actor_SetRotationZ)> _Actor_SetRotationZ;
	};


	inline void alloc() 
	{
		SKSE::AllocTrampoline(1 << 8);
		//SKSE::AllocTrampoline(1 << 4);
	}

};
