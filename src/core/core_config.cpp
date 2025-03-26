#include "core_config.hpp"
#include <core/sdk/utils.h>
#include <plugify-configs/plugify-configs.hpp>

using namespace std::string_view_literals;

CoreConfig::CoreConfig(plg::vector<plg::string> paths) : m_paths(std::move(paths)) {
}

bool CoreConfig::Initialize() {
	std::vector<std::string_view> paths;
	paths.reserve(m_paths.size());
	for (const auto& path : m_paths) {
		paths.emplace_back(path);
	}
	auto config = ReadConfigs(paths);
	if (!config) {
		S2_LOGF(LS_ERROR, "Failed to load configuration file: \"%s\"\n", GetError().data());
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

	FollowCS2ServerGuidelines = config->GetBool("FollowCS2ServerGuidelines");
	ServerLanguage = config->GetString("ServerLanguage", "en");

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