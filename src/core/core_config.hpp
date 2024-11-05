#pragma once

#include <tier0/keyvalues.h>

class CCoreConfig
{
public:
	explicit CCoreConfig(plg::string path);
	~CCoreConfig() = default;

	bool Initialize();

	const plg::string& GetPath() const;

	bool IsSilentChatTrigger(const plg::string& message, plg::string& prefix) const;
	bool IsPublicChatTrigger(const plg::string& message, plg::string& prefix) const;

private:
	bool IsTriggerInternal(const plg::vector<plg::string>& triggers, const plg::string& message, plg::string& prefix) const;

public:
	plg::vector<plg::string> PublicChatTrigger{"!"};
	plg::vector<plg::string> SilentChatTrigger{"/"};
	bool FollowCS2ServerGuidelines{true};
	plg::string ServerLanguage{"en"};

private:
	plg::string m_szPath;
	std::unique_ptr<KeyValues> m_pKeyValues;
};
