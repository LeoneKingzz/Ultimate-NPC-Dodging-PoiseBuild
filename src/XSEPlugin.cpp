#include "hooks.h"
//#include "log.h"
#include "settings.h"

void OnDataLoaded()
{
   
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		hooks::animEventHandler::Register(true, true);
		hooks::IHooks::install();
		// hooks::on_set_rotation::install();
		if (settings::bTacticalDodgeAI_enable)
		{ // install hooks for passive dodge
			hooks::on_combatBehavior_backoff_createPath::install();
			hooks::on_combatBehavior_circle_createPath::install();
			hooks::on_combatBehavior_dodgethreat_createPath::install();
			hooks::on_combatBehavior_fallback_createPath::install();
		}
		settings::init();
		break;

	case SKSE::MessagingInterface::kPostPostLoad:
		dodge::GetSingleton()->init();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		settings::setglobals();
		break;
	default:
		break;
	}
}

void Init()
{
	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", MessageHandler);
}

void onSKSEInit()
{
	settings::read(L"Data/SKSE/Plugins/UltimateNPCDodging.ini");
	hooks::alloc();
}

void InitializeLog()
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		util::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
	const auto level = spdlog::level::trace;
#else
	const auto level = spdlog::level::info;
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);
}

EXTERN_C [[maybe_unused]] __declspec(dllexport) bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface *a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent())
	{
	};
#endif

	InitializeLog();

	logger::info("Loaded plugin");

	SKSE::Init(a_skse);

	Init();

	onSKSEInit();

	return true;
}

EXTERN_C [[maybe_unused]] __declspec(dllexport) constinit auto SKSEPlugin_Version = []() noexcept
{
	SKSE::PluginVersionData v;
	v.PluginName("PluginName");
	v.PluginVersion({1, 0, 0, 0});
	v.UsesAddressLibrary(true);
	v.HasNoStructUse(true);
	return v;
}();

EXTERN_C [[maybe_unused]] __declspec(dllexport) bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface *, SKSE::PluginInfo *pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}