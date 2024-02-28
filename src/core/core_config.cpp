#include "core_config.h"
#include "utils.h"

CCoreConfig::CCoreConfig(std::string path) : m_szPath(std::move(path)), m_pKeyValues(std::make_unique<KeyValues>("Core"))
{
}
bool CCoreConfig::Initialize(std::span<char> error)
{
	if (!m_pKeyValues->LoadFromFile(g_pFullFileSystem, m_szPath.c_str(), nullptr))
	{
		snprintf(error.data(), error.size(), "Failed to load gamedata file");
		return false;
	}

	KeyValues* publicChatTriggers = m_pKeyValues->FindKey("PublicChatTrigger");
	if (publicChatTriggers)
	{
		PublicChatTrigger.clear();

		FOR_EACH_SUBKEY(publicChatTriggers, it)
		{
			PublicChatTrigger.emplace_back(it->GetString("trigger"));
		}
	}

	KeyValues* silentChatTriggers = m_pKeyValues->FindKey("SilentChatTrigger");
	if (silentChatTriggers)
	{
		SilentChatTrigger.clear();

		FOR_EACH_SUBKEY(silentChatTriggers, it)
		{
			SilentChatTrigger.emplace_back(it->GetString("trigger"));
		}
	}

	FollowCS2ServerGuidelines = m_pKeyValues->GetBool("FollowCS2ServerGuidelines");
	ServerLanguage = m_pKeyValues->GetString("ServerLanguage", "en");

	return true;
}

const std::string& CCoreConfig::GetPath() const
{
	return m_szPath;
}

bool CCoreConfig::IsTriggerInternal(const std::vector<std::string>& triggers, const std::string& message, std::string& prefix) const
{
	for (const std::string& trigger : triggers)
	{
		if (message.rfind(trigger, 0) == 0)
		{
			prefix = trigger;
			g_Logger.MessageFormat("Trigger found, prefix is %s\n", prefix.c_str());
			return true;
		}
	}

	return false;
}

bool CCoreConfig::IsSilentChatTrigger(const std::string& message, std::string& prefix) const
{
	return IsTriggerInternal(SilentChatTrigger, message, prefix);
}

bool CCoreConfig::IsPublicChatTrigger(const std::string& message, std::string& prefix) const
{
	return IsTriggerInternal(PublicChatTrigger, message, prefix);
}