#pragma once

#include <regex>

class CoreConfig {
public:
	explicit CoreConfig(plg::vector<plg::string> paths);
	~CoreConfig();

	bool Initialize();

	const plg::vector<plg::string>& GetPaths() const;

	bool IsSilentChatTrigger(std::string_view message) const;
	bool IsPublicChatTrigger(std::string_view message) const;
	bool IsRegexMatch(std::string_view message) const;

private:
	static bool IsTriggerInternal(const std::vector<plg::string>& triggers, std::string_view message);

public:
	std::vector<plg::string> PublicChatTrigger{"!"};
	std::vector<plg::string> SilentChatTrigger{"/"};
	std::vector<std::regex> FilterConsoleCleaner{};
	plg::string ServerLanguage{"en"};
	bool FollowCS2ServerGuidelines{true};

private:
	plg::vector<plg::string> m_paths;
};
