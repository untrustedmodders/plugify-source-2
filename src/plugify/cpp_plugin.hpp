#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <filesystem>
#include <utility>
#include <vector>

#include <plugify/string.hpp>
#include <plugify/vector.hpp>

namespace std::filesystem {
#if _WIN32
	using path_view = std::wstring_view;
#else
	using path_view = std::string_view;
#endif
}

namespace plg {
	constexpr int32_t kApiVersion = 1;

	extern "C" struct PluginResult {
		int32_t version;
		bool debug;
	};

	using GetMethodPtrFn = void* (*)(std::string_view);
	using GetMethodPtr2Fn = void (*)(std::string_view, void**);
	using GetBaseDirFn = std::filesystem::path_view (*)();
	using IsModuleLoadedFn = bool (*)(std::string_view, std::optional<int32_t>, bool);
	using IsPluginLoadedFn = bool (*)(std::string_view, std::optional<int32_t>, bool);

	extern GetMethodPtrFn GetMethodPtr;
	extern GetMethodPtr2Fn GetMethodPtr2;
	extern GetBaseDirFn GetBaseDir;
	extern IsModuleLoadedFn IsModuleLoaded;
	extern IsPluginLoadedFn IsPluginLoaded;

	namespace plugin {
		using GetIdFn = std::ptrdiff_t (*)(void*);
		using GetNameFn = std::string_view (*)(void*);
		using GetFullNameFn = std::string_view (*)(void*);
		using GetDescriptionFn = std::string_view (*)(void*);
		using GetVersionFn = std::string_view (*)(void*);
		using GetAuthorFn = std::string_view (*)(void*);
		using GetWebsiteFn = std::string_view (*)(void*);
		using GetBaseDirFn = std::filesystem::path_view (*)(void*);
		using GetDependenciesFn = std::vector<std::string_view> (*)(void*);
		using FindResourceFn = std::optional<std::filesystem::path_view> (*)(void*, std::filesystem::path_view);
		extern void* handle;
		extern GetIdFn GetId;
		extern GetNameFn GetName;
		extern GetFullNameFn GetFullName;
		extern GetDescriptionFn GetDescription;
		extern GetVersionFn GetVersion;
		extern GetAuthorFn GetAuthor;
		extern GetWebsiteFn GetWebsite;
		extern GetBaseDirFn GetBaseDir;
		extern GetDependenciesFn GetDependencies;
		extern FindResourceFn FindResource;
	}

	class IPluginEntry {
	protected:
		IPluginEntry() = default;
		~IPluginEntry() = default;

	public:
		std::ptrdiff_t GetId() const { return plugin::GetId(plugin::handle); }
		std::string_view GetName() const { return plugin::GetName(plugin::handle); }
		std::string_view GetFullName() const { return plugin::GetFullName(plugin::handle); }
		std::string_view GetDescription() const { return plugin::GetDescription(plugin::handle); }
		std::string_view GetVersion() const { return plugin::GetVersion(plugin::handle); }
		std::string_view GetAuthor() const { return plugin::GetAuthor(plugin::handle); }
		std::string_view GetWebsite() const { return plugin::GetWebsite(plugin::handle); }
		std::filesystem::path_view GetBaseDir() const { return plugin::GetBaseDir(plugin::handle); }
		std::vector<std::string_view> GetDependencies() const { return plugin::GetDependencies(plugin::handle); }
		std::optional<std::filesystem::path_view> FindResource(std::filesystem::path_view path) const { return plugin::FindResource(plugin::handle, path); }

		virtual void OnPluginStart() {};
		virtual void OnPluginEnd() {};
	};

	IPluginEntry* GetPluginEntry();
}

#define EXPOSE_PLUGIN(plugin_api, interface_addr) namespace plg { \
	GetMethodPtrFn GetMethodPtr{ nullptr }; \
	GetMethodPtr2Fn GetMethodPtr2{ nullptr }; \
	GetBaseDirFn GetBaseDir{ nullptr }; \
	IsModuleLoadedFn IsModuleLoaded{ nullptr }; \
	IsPluginLoadedFn IsPluginLoaded{ nullptr }; \
	namespace plugin { \
		void* handle{ nullptr }; \
		GetIdFn GetId{ nullptr }; \
		GetNameFn GetName{ nullptr }; \
		GetFullNameFn GetFullName{ nullptr }; \
		GetDescriptionFn GetDescription{ nullptr }; \
		GetVersionFn GetVersion{ nullptr }; \
		GetAuthorFn GetAuthor{ nullptr }; \
		GetWebsiteFn GetWebsite{ nullptr }; \
		GetBaseDirFn GetBaseDir{ nullptr }; \
		GetDependenciesFn GetDependencies{ nullptr }; \
		FindResourceFn FindResource{ nullptr }; \
	} \
	extern "C" \
	plugin_api PluginResult Plugify_Init(void** api, int32_t version, void* handle) { \
		if (version < kApiVersion) { \
			return { kApiVersion, PLUGIFY_IS_DEBUG }; \
		} \
		std::size_t i = 0; \
		GetMethodPtr = reinterpret_cast<GetMethodPtrFn>(api[i++]); \
		GetMethodPtr2 = reinterpret_cast<GetMethodPtr2Fn>(api[i++]); \
		GetBaseDir = reinterpret_cast<GetBaseDirFn>(api[i++]); \
		IsModuleLoaded = reinterpret_cast<IsModuleLoadedFn>(api[i++]); \
		IsPluginLoaded = reinterpret_cast<IsPluginLoadedFn>(api[i++]); \
		plugin::GetId = reinterpret_cast<plugin::GetIdFn>(api[i++]); \
		plugin::GetName = reinterpret_cast<plugin::GetNameFn>(api[i++]); \
		plugin::GetFullName = reinterpret_cast<plugin::GetFullNameFn>(api[i++]); \
		plugin::GetDescription = reinterpret_cast<plugin::GetDescriptionFn>(api[i++]); \
		plugin::GetVersion = reinterpret_cast<plugin::GetVersionFn>(api[i++]); \
		plugin::GetAuthor = reinterpret_cast<plugin::GetAuthorFn>(api[i++]); \
		plugin::GetWebsite = reinterpret_cast<plugin::GetWebsiteFn>(api[i++]); \
		plugin::GetBaseDir = reinterpret_cast<plugin::GetBaseDirFn>(api[i++]); \
		plugin::GetDependencies = reinterpret_cast<plugin::GetDependenciesFn>(api[i++]); \
		plugin::FindResource = reinterpret_cast<plugin::FindResourceFn>(api[i++]); \
		plugin::handle = handle; \
		return { 0, PLUGIFY_IS_DEBUG }; \
	} \
	extern "C" \
	plugin_api void Plugify_PluginStart() { \
		GetPluginEntry()->OnPluginStart(); \
	} \
	extern "C" \
	plugin_api void Plugify_PluginEnd() { \
		GetPluginEntry()->OnPluginEnd(); \
	} \
	plg::IPluginEntry* GetPluginEntry() { \
		return interface_addr; \
	} \
}

namespace plg {
	extern "C" {
		struct vec2 {
			float x;
			float y;
		};

		struct vec3 {
			float x;
			float y;
			float z;
		};

		struct vec4 {
			float x;
			float y;
			float z;
			float w;
		};

		struct mat4x4 {
			float m[4][4];
		};

		struct vec {
			[[maybe_unused]] char padding[sizeof(plg::vector<int>)];
		};

		struct str {
			[[maybe_unused]] char padding[sizeof(plg::string)];
		};
	}

	[[nodiscard]] constexpr bool operator==(const vec2& lhs, const vec2& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}

	[[nodiscard]] constexpr bool operator==(const vec3& lhs, const vec3& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	[[nodiscard]] constexpr bool operator==(const vec4& lhs, const vec4& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
	}

	[[nodiscard]] constexpr bool operator==(const mat4x4& lhs, const mat4x4& rhs) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (lhs.m[i][j] != rhs.m[i][j])
					return false;
			}
		}
		return true;
	}

    [[nodiscard]] inline plg::str ReturnStr(plg::string str) {
        plg::str ret{};
        std::construct_at(reinterpret_cast<plg::string*>(&ret), std::move(str));
        return ret;
    }

    template<typename T>
    [[nodiscard]] inline plg::vec ReturnVec(plg::vector<T> vec) {
        plg::vec ret{};
        std::construct_at(reinterpret_cast<plg::vector<T>*>(&ret), std::move(vec));
        return ret;
    }
}
