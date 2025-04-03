#pragma once

#if S2SDK_PLATFORM_WINDOWS
#include <regex>
using regex = std::regex;
#else
namespace re2 {
	class RE2;
}
using regex = re2::RE2;
#endif

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
	static bool IsTriggerInternal(const std::vector<std::string>& triggers, std::string_view message);

public:
	std::vector<std::string> PublicChatTrigger{"!"};
	std::vector<std::string> SilentChatTrigger{"/"};
	std::vector<regex*> FilterConsoleCleaner{};
	std::string ServerLanguage{"en"};
	std::vector<uint64_t> ExtraAddons{};
	double RejoinTimeout{10.0};
	bool AddonMountDownload{};
	bool CacheClientsWithAddons{};
	bool BlockDisconnectMessages{};
	bool FollowCS2ServerGuidelines{true};

private:
	plg::vector<plg::string> m_paths;
};
