#pragma once

#include <re2/re2.h>

class CoreConfig {
public:
	explicit CoreConfig(plg::vector<plg::string> paths);
	~CoreConfig() = default;

	bool Initialize();

	const plg::vector<plg::string>& GetPaths() const;

	bool IsSilentChatTrigger(std::string_view message) const;
	bool IsPublicChatTrigger(std::string_view message) const;
	bool IsRegexMatch(std::string_view message) const;

private:
	static bool IsTriggerInternal(const std::vector<std::string>& triggers, std::string_view message);

public:
	std::vector<std::string> PublicChatTrigger{"!"};
	std::vector<std::string> SilentChatTrigger{"/"};
	std::vector<std::unique_ptr<re2::RE2>> FilterConsoleCleaner{};
	bool FollowCS2ServerGuidelines{true};
	std::string ServerLanguage{"en"};

private:
	plg::vector<plg::string> m_paths;
};
