#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <filesystem>
#include <utility>
#include <vector>
#include <type_traits>

#include "any.hpp"
#include "version.hpp"

#include "api/plugify_api.hpp"

namespace std::filesystem {
#if _WIN32
	using path_view = std::wstring_view;
#else
	using path_view = std::string_view;
#endif
} // namespace std::filesystem

namespace plg {
	using GetMethodPtrFn = void* (*)(std::string_view);
	using GetMethodPtr2Fn = void (*)(std::string_view, void**);
	using GetBaseDirFn = std::filesystem::path_view (*)();
	using IsModuleLoadedFn = bool (*)(std::string_view, std::optional<plg::version>, bool);
	using IsPluginLoadedFn = bool (*)(std::string_view, std::optional<plg::version>, bool);

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
		virtual void OnPluginUpdate(float) {};
		virtual void OnPluginEnd() {};
	};

	IPluginEntry* GetPluginEntry();
} // namespace plg

#define EXPOSE_PLUGIN(plugin_api, plugin_class, plugin_addr) \
    namespace plg { \
        GetMethodPtrFn GetMethodPtr{nullptr}; \
        GetMethodPtr2Fn GetMethodPtr2{nullptr}; \
        GetBaseDirFn GetBaseDir{nullptr}; \
        IsModuleLoadedFn IsModuleLoaded{nullptr}; \
        IsPluginLoadedFn IsPluginLoaded{nullptr}; \
        namespace plugin { \
            void* handle{nullptr}; \
            GetIdFn GetId{nullptr}; \
            GetNameFn GetName{nullptr}; \
            GetFullNameFn GetFullName{nullptr}; \
            GetDescriptionFn GetDescription{nullptr}; \
            GetVersionFn GetVersion{nullptr}; \
            GetAuthorFn GetAuthor{nullptr}; \
            GetWebsiteFn GetWebsite{nullptr}; \
            GetBaseDirFn GetBaseDir{nullptr}; \
            GetDependenciesFn GetDependencies{nullptr}; \
            FindResourceFn FindResource{nullptr}; \
        } \
        extern "C" plugin_api int Plugify_Init(void** api, int version, void* handle) { \
            if (version < kApiVersion) { \
                return kApiVersion; \
            } \
            size_t i = 0; \
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
                                                 decltype(&plugin_class::OnPluginStart)>> {}; \
        template<typename T, typename = void> \
        struct has_overridden_OnPluginUpdate : std::false_type {}; \
        template<typename T> \
        struct has_overridden_OnPluginUpdate<T, std::void_t<decltype(std::declval<T>().OnPluginUpdate(0.0f))>> \
            : std::bool_constant<!std::is_same_v<decltype(&T::OnPluginUpdate), \
                                                 decltype(&plugin_class::OnPluginUpdate)>> {}; \
        template<typename T, typename = void> \
        struct has_overridden_OnPluginEnd : std::false_type {}; \
        template<typename T> \
        struct has_overridden_OnPluginEnd<T, std::void_t<decltype(std::declval<T>().OnPluginEnd())>> \
            : std::bool_constant<!std::is_same_v<decltype(&T::OnPluginEnd), \
                                                 decltype(&plugin_class::OnPluginEnd)>> {}; \
        extern "C" plugin_api void Plugify_PluginStart() { \
            GetPluginEntry()->OnPluginStart(); \
        } \
        extern "C" plugin_api void Plugify_PluginUpdate(float dt) { \
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
