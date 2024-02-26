#pragma once

#include <tier0/KeyValues.h>

class CCoreConfig
{
public:
	explicit CCoreConfig(std::string path);
	~CCoreConfig() = default;

	bool Initialize(std::span<char> error);

	const std::string& GetPath() const;

	bool IsSilentChatTrigger(const std::string& message, std::string& prefix) const;
	bool IsPublicChatTrigger(const std::string& message, std::string& prefix) const;

private:
	bool IsTriggerInternal(std::vector<std::string> triggers, const std::string& message, std::string& prefix) const;

public:
	std::vector<std::string> PublicChatTrigger{ "!" };
	std::vector<std::string> SilentChatTrigger{ "/" };
	bool FollowCS2ServerGuidelines{ true };
	std::string ServerLanguage{ "en" };

private:
	std::string m_szPath;
	std::unique_ptr<KeyValues> m_pKeyValues;
};
