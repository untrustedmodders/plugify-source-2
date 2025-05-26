#include "core_config.hpp"
#include <core/sdk/utils.h>
#include <plugify-configs/plugify-configs.hpp>

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
		S2_LOGF(LS_ERROR, "Failed to load configuration file: \"{}\"\n", pcf::GetError());
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

	FilterConsoleCleaner.clear();
	if (config->JumpKey("FilterConsoleCleaner")) {
		if (config->IsArray() && config->JumpFirst()) {
			do {
				plg::string pattern = config->GetString();
				FilterConsoleCleaner.emplace_back(pattern.begin(), pattern.end());
			} while (config->JumpNext());
			config->JumpBack();
		}
		config->JumpBack();
	}

	ServerLanguage = config->GetString("ServerLanguage", "en");
	FollowCS2ServerGuidelines = config->GetBool("FollowCS2ServerGuidelines", true);

	return true;
}

const plg::vector<plg::string>& CoreConfig::GetPaths() const {
	return m_paths;
}

bool CoreConfig::IsTriggerInternal(const std::vector<plg::string>& triggers, std::string_view message) {
	for (const plg::string& trigger : triggers) {
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
	for (const auto& regex : FilterConsoleCleaner) {
		if (std::regex_match(message.begin(), message.end(), regex))
			return true;
	}
	return false;
}