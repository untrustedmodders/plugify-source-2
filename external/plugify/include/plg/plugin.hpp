#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <filesystem>
#include <utility>
#include <vector>
#include <chrono>
#include <type_traits>

#include "plg/any.hpp"
#include "plg/version.hpp"
#include "plg/api.hpp"

namespace plg {
	using GetMethodPtrFn = void* (*)(std::string_view);
	using GetMethodPtr2Fn = void (*)(std::string_view, void**);
	using GetBaseDirFn = plg::string (*)();
	using GetExtensionsDirFn = plg::string (*)();
	using GetConfigsDirFn = plg::string (*)();
	using GetDataDirFn = plg::string (*)();
	using GetLogsDirFn = plg::string (*)();
	using GetCacheDirFn = plg::string (*)();
	using IsExtensionLoadedFn = bool (*)(std::string_view, std::optional<plg::range_set<>>);

	extern GetMethodPtrFn GetMethodPtr;
	extern GetMethodPtr2Fn GetMethodPtr2;
	extern GetBaseDirFn GetBaseDir;
	extern GetExtensionsDirFn GetExtensionsDir;
	extern GetConfigsDirFn GetConfigsDir;
	extern GetDataDirFn GetDataDir;
	extern GetLogsDirFn GetLogsDir;
	extern GetCacheDirFn GetCacheDir;
	extern IsExtensionLoadedFn IsExtensionLoaded;

	struct Dependency {
		plg::string name;
		plg::range_set<> constraints;
		bool optional;
	};

	using uuid = std::ptrdiff_t;

	namespace plugin {
		using GetIdFn = plg::uuid (*)(void*);
		using GetNameFn = plg::string (*)(void*);
		using GetLicenseFn = plg::string (*)(void*);
		using GetDescriptionFn = plg::string (*)(void*);
		using GetVersionFn = plg::version<> (*)(void*);
		using GetAuthorFn = plg::string (*)(void*);
		using GetWebsiteFn = plg::string (*)(void*);
		using GetLocationFn = plg::string (*)(void*);
		using GetDependenciesFn = plg::vector<plg::Dependency> (*)(void*);
		extern void* handle;
		extern GetIdFn GetId;
		extern GetNameFn GetName;
		extern GetDescriptionFn GetDescription;
		extern GetVersionFn GetVersion;
		extern GetAuthorFn GetAuthor;
		extern GetWebsiteFn GetWebsite;
		extern GetLicenseFn GetLicense;
		extern GetLocationFn GetLocation;
		extern GetDependenciesFn GetDependencies;
	}

	class IPluginEntry {
	protected:
		IPluginEntry() = default;
		~IPluginEntry() = default;

	public:
		plg::uuid GetId() const { return plugin::GetId(plugin::handle); }
		plg::string GetName() const { return plugin::GetName(plugin::handle); }
		plg::string GetDescription() const { return plugin::GetDescription(plugin::handle); }
		plg::version<> GetVersion() const { return plugin::GetVersion(plugin::handle); }
		plg::string GetAuthor() const { return plugin::GetAuthor(plugin::handle); }
		plg::string GetWebsite() const { return plugin::GetWebsite(plugin::handle); }
		plg::string GetLicense() const { return plugin::GetLicense(plugin::handle); }
		plg::string GetLocation() const { return plugin::GetLocation(plugin::handle); }
		plg::vector<plg::Dependency> GetDependencies() const { return plugin::GetDependencies(plugin::handle); }

		virtual void OnPluginStart() {};
		virtual void OnPluginUpdate(std::chrono::milliseconds) {};
		virtual void OnPluginEnd() {};
	};

	IPluginEntry* GetPluginEntry();
} // namespace plg

#define EXPOSE_PLUGIN(plugin_api, plugin_class, plugin_addr) \
    namespace plg { \
        GetMethodPtrFn GetMethodPtr{nullptr}; \
        GetMethodPtr2Fn GetMethodPtr2{nullptr}; \
		GetBaseDirFn GetBaseDir{nullptr}; \
		GetExtensionsDirFn GetExtensionsDir{nullptr}; \
		GetConfigsDirFn GetConfigsDir{nullptr}; \
		GetDataDirFn GetDataDir{nullptr}; \
		GetLogsDirFn GetLogsDir{nullptr}; \
		GetCacheDirFn GetCacheDir{nullptr}; \
        IsExtensionLoadedFn IsExtensionLoaded{nullptr}; \
        namespace plugin { \
            void* handle{nullptr}; \
            GetIdFn GetId{nullptr}; \
            GetNameFn GetName{nullptr}; \
            GetDescriptionFn GetDescription{nullptr}; \
            GetVersionFn GetVersion{nullptr}; \
            GetAuthorFn GetAuthor{nullptr}; \
            GetWebsiteFn GetWebsite{nullptr}; \
            GetLicenseFn GetLicense{nullptr}; \
            GetLocationFn GetLocation{nullptr}; \
            GetDependenciesFn GetDependencies{nullptr}; \
        } \
        extern "C" plugin_api int Plugify_Init(void** api, int version, void* handle) { \
            if (version < kApiVersion) { \
                return kApiVersion; \
            } \
            size_t i = 0; \
            GetMethodPtr = reinterpret_cast<GetMethodPtrFn>(api[i++]); \
            GetMethodPtr2 = reinterpret_cast<GetMethodPtr2Fn>(api[i++]); \
            GetBaseDir = reinterpret_cast<GetBaseDirFn>(api[i++]); \
            GetExtensionsDir = reinterpret_cast<GetExtensionsDirFn>(api[i++]); \
            GetConfigsDir = reinterpret_cast<GetConfigsDirFn>(api[i++]); \
            GetDataDir = reinterpret_cast<GetDataDirFn>(api[i++]); \
            GetLogsDir = reinterpret_cast<GetLogsDirFn>(api[i++]); \
            GetCacheDir = reinterpret_cast<GetCacheDirFn>(api[i++]); \
            IsExtensionLoaded = reinterpret_cast<IsExtensionLoadedFn>(api[i++]); \
            plugin::GetId = reinterpret_cast<plugin::GetIdFn>(api[i++]); \
            plugin::GetName = reinterpret_cast<plugin::GetNameFn>(api[i++]); \
            plugin::GetDescription = reinterpret_cast<plugin::GetDescriptionFn>(api[i++]); \
            plugin::GetVersion = reinterpret_cast<plugin::GetVersionFn>(api[i++]); \
            plugin::GetAuthor = reinterpret_cast<plugin::GetAuthorFn>(api[i++]); \
            plugin::GetWebsite = reinterpret_cast<plugin::GetWebsiteFn>(api[i++]); \
            plugin::GetLicense = reinterpret_cast<plugin::GetLicenseFn>(api[i++]); \
            plugin::GetLocation = reinterpret_cast<plugin::GetLocationFn>(api[i++]); \
            plugin::GetDependencies = reinterpret_cast<plugin::GetDependenciesFn>(api[i++]); \
            plugin::handle = handle; \
            return 0; \
        } \
        plg::IPluginEntry* GetPluginEntry() { \
            return plugin_addr; \
        } \
        template<typename T, typename = void> \
        struct has_overridden_OnPluginStart : std::false_type {}; \
        template<typename T> \
        struct has_overridden_OnPluginStart<T, std::void_t<decltype(std::declval<T>().OnPluginStart())>> \
            : std::bool_constant<!std::is_same_v<decltype(&T::OnPluginStart), \
                                                 decltype(&IPluginEntry::OnPluginStart)>> {}; \
        template<typename T, typename = void> \
        struct has_overridden_OnPluginUpdate : std::false_type {}; \
        template<typename T> \
        struct has_overridden_OnPluginUpdate<T, std::void_t<decltype(std::declval<T>().OnPluginUpdate(std::chrono::milliseconds{0}))>> \
            : std::bool_constant<!std::is_same_v<decltype(&T::OnPluginUpdate), \
                                                 decltype(&IPluginEntry::OnPluginUpdate)>> {}; \
        template<typename T, typename = void> \
        struct has_overridden_OnPluginEnd : std::false_type {}; \
        template<typename T> \
        struct has_overridden_OnPluginEnd<T, std::void_t<decltype(std::declval<T>().OnPluginEnd())>> \
            : std::bool_constant<!std::is_same_v<decltype(&T::OnPluginEnd), \
                                                 decltype(&IPluginEntry::OnPluginEnd)>> {}; \
        extern "C" plugin_api void Plugify_PluginStart() { \
            GetPluginEntry()->OnPluginStart(); \
        } \
        extern "C" plugin_api void Plugify_PluginUpdate(std::chrono::milliseconds dt) { \
            GetPluginEntry()->OnPluginUpdate(dt); \
        } \
        extern "C" plugin_api void Plugify_PluginEnd() { \
            GetPluginEntry()->OnPluginEnd(); \
        } \
        extern "C" plugin_api PluginContext* Plugify_PluginContext() { \
            static PluginContext context = { \
                .hasUpdate = has_overridden_OnPluginUpdate<plugin_class>::value, \
                .hasStart = has_overridden_OnPluginStart<plugin_class>::value, \
                .hasEnd = has_overridden_OnPluginEnd<plugin_class>::value, \
                .hasDebug = PLUGIFY_IS_DEBUG \
            }; \
            return &context; \
        } \
    }

namespace plg {
	namespace raw {
		struct vector {
			[[maybe_unused]] uint8_t padding[sizeof(plg::vector<int>)]{};
		};

		struct string {
			[[maybe_unused]] uint8_t padding[sizeof(plg::string)]{};
		};

		struct variant {
			[[maybe_unused]] uint8_t padding[sizeof(plg::any)]{};
		};
	} // namespace raw

	template<typename T, typename V>
	[[nodiscard]] PLUGIFY_FORCE_INLINE T as_raw(V&& value) {
		T ret{};
		std::construct_at(reinterpret_cast<V*>(&ret), std::forward<V>(value));
		return ret;
	}
} // namespace plg
