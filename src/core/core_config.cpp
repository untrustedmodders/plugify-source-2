#include "core_config.hpp"
#include <core/sdk/utils.h>
#include <plugify-configs/plugify-configs.hpp>

/*#if S2SDK_PLATFORM_WINDOWS
#include <regex>
#else
#undef POSIX
#include <re2/re2.h>
#endif*/

using namespace std::string_view_literals;

CoreConfig::CoreConfig(plg::vector<plg::string> paths) : m_paths(std::move(paths)) {
}

CoreConfig::~CoreConfig() {
	/*for (auto& regex : FilterConsoleCleaner) {
		delete regex;
	}*/
}

bool CoreConfig::Initialize() {
	std::vector<std::string_view> paths;
	paths.reserve(m_paths.size());
	for (const auto& path : m_paths) {
		paths.emplace_back(path);
	}
	auto config = pcf::ReadConfigs(paths);
	if (!config) {
		S2_LOGF(LS_ERROR, "Failed to load configuration file: \"%s\"\n", pcf::GetError().data());
		return false;
	}

	PublicChatTrigger.clear();
	if (config->JumpKey("PublicChatTrigger")) {
		if (config->IsArray() && config->JumpFirst()) {
			do {
				PublicChatTrigger.emplace_back(config->GetString());
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	SilentChatTrigger.clear();
	if (config->JumpKey("SilentChatTrigger")) {
		if (config->IsArray() && config->JumpFirst()) {
			do {
				SilentChatTrigger.emplace_back(config->GetString());
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	/*FilterConsoleCleaner.clear();
	if (config->JumpKey("FilterConsoleCleaner")) {
		if (config->IsArray() && config->JumpFirst()) {
			do {
#if S2SDK_PLATFORM_WINDOWS
				plg::string pattern = config->GetString();
				FilterConsoleCleaner.emplace_back(new std::regex(pattern.begin(), pattern.end()));
#else
				RE2::Options options;
				options.set_dot_nl(true);
				FilterConsoleCleaner.emplace_back(new re2::RE2(config->GetString(), options));
#endif
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}*/

	ServerLanguage = config->GetString("ServerLanguage", "en");

	ExtraAddons.clear();
	if (config->JumpKey("ExtraAddons")) {
		if (config->IsArray() && config->JumpFirst()) {
			do {
				uint64_t value = static_cast<uint64_t>(config->GetAsInt64());
				if (std::find(ExtraAddons.begin(), ExtraAddons.end(), value) == ExtraAddons.end()) {
					ExtraAddons.emplace_back(value);
				}
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	RejoinTimeout = config->GetDouble("RejoinTimeout", 10.0);
	AddonMountDownload = config->GetBool("AddonMountDownload", false);
	CacheClientsWithAddons = config->GetBool("CacheClientsWithAddons", false);
	BlockDisconnectMessages = config->GetBool("BlockDisconnectMessages", false);

	FollowCS2ServerGuidelines = config->GetBool("FollowCS2ServerGuidelines", true);

	return true;
}

const plg::vector<plg::string>& CoreConfig::GetPaths() const {
	return m_paths;
}

bool CoreConfig::IsTriggerInternal(const std::vector<std::string>& triggers, std::string_view message) {
	for (const std::string& trigger : triggers) {
		if (message.starts_with(trigger)) {
			return true;
		}
	}

	return false;
}

bool CoreConfig::IsSilentChatTrigger(std::string_view message) const {
	return IsTriggerInternal(SilentChatTrigger, message);
}

bool CoreConfig::IsPublicChatTrigger(std::string_view message) const {
	return IsTriggerInternal(PublicChatTrigger, message);
}

bool CoreConfig::IsRegexMatch(std::string_view message) const {
	/*for (auto& regex : FilterConsoleCleaner) {
#if S2SDK_PLATFORM_WINDOWS
		if (std::regex_match(message.begin(), message.end(), *regex))
#else
		if (RE2::FullMatch(message, *regex))
#endif
			return true;
	}*/
	return false;
}