#pragma once

#include <tier0/keyvalues.h>

class CCoreConfig {
public:
	explicit CCoreConfig(plg::string path);
	~CCoreConfig() = default;

	bool Initialize();

	const plg::string& GetPath() const;

	bool IsSilentChatTrigger(std::string_view message) const;
	bool IsPublicChatTrigger(std::string_view message) const;

private:
	static bool IsTriggerInternal(const std::vector<std::string>& triggers, std::string_view message);

public:
	std::vector<std::string> PublicChatTrigger{"!"};
	std::vector<std::string> SilentChatTrigger{"/"};
	bool FollowCS2ServerGuidelines{true};
	std::string ServerLanguage{"en"};

private:
	plg::string m_szPath;
	std::unique_ptr<KeyValues> m_pKeyValues;
};
