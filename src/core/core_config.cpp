#include "core_config.hpp"
#include <core/sdk/utils.h>

CCoreConfig::CCoreConfig(plg::string path) : m_szPath(std::move(path)), m_pKeyValues(std::make_unique<KeyValues>("Core")) {
}

bool CCoreConfig::Initialize() {
	if (!m_pKeyValues->LoadFromFile(g_pFullFileSystem, m_szPath.c_str(), nullptr)) {
		S2_LOGF(LS_ERROR, "Could not read \"%s\": Failed to load gamedata file\n", m_szPath.c_str());
		return false;
	}

	KeyValues* publicChatTriggers = m_pKeyValues->FindKey("PublicChatTrigger");
	if (publicChatTriggers) {
		PublicChatTrigger.clear();

		FOR_EACH_SUBKEY(publicChatTriggers, it) {
			PublicChatTrigger.emplace_back(it->GetString("trigger"));
		}
	}

	KeyValues* silentChatTriggers = m_pKeyValues->FindKey("SilentChatTrigger");
	if (silentChatTriggers) {
		SilentChatTrigger.clear();

		FOR_EACH_SUBKEY(silentChatTriggers, it) {
			SilentChatTrigger.emplace_back(it->GetString("trigger"));
		}
	}

	FollowCS2ServerGuidelines = m_pKeyValues->GetBool("FollowCS2ServerGuidelines");
	ServerLanguage = m_pKeyValues->GetString("ServerLanguage", "en");

	return true;
}

const plg::string& CCoreConfig::GetPath() const {
	return m_szPath;
}

bool CCoreConfig::IsTriggerInternal(const std::vector<std::string>& triggers, std::string_view message) {
	for (const std::string& trigger: triggers) {
		return message.rfind(trigger, 0) == 0;
	}

	return false;
}

bool CCoreConfig::IsSilentChatTrigger(std::string_view message) const {
	return IsTriggerInternal(SilentChatTrigger, message);
}

bool CCoreConfig::IsPublicChatTrigger(std::string_view message) const {
	return IsTriggerInternal(PublicChatTrigger, message);
}