#pragma once

#include <string>
#include <cstdint>
#include <filesystem>
#include <optional>

#undef FindResource

namespace plugify {
	constexpr int kApiVersion = 1;

	using InitFunc = int (*)(const void* const*, int, const void*);
	using StartFunc = void (*)();
	using EndFunc = void (*)();

	using GetMethodPtrFn = void* (*)(const std::string&);
	using GetBaseDirFn = const std::filesystem::path& (*)();
	using IsModuleLoadedFn = bool (*)(const std::string&, std::optional<int32_t>, bool);
	using IsPluginLoadedFn = bool (*)(const std::string&, std::optional<int32_t>, bool);

	extern GetMethodPtrFn GetMethodPtr;
	extern GetBaseDirFn GetBaseDir;
	extern IsModuleLoadedFn IsModuleLoaded;
	extern IsPluginLoadedFn IsPluginLoaded;

	namespace plugin {
		extern void* handle;

		using GetIdFn = std::ptrdiff_t (*)(void*);
		using GetNameFn = const std::string& (*)(void*);
		using GetFullNameFn = const std::string& (*)(void*);
		using GetDescriptionFn = const std::string& (*)(void*);
		using GetVersionFn = const std::string& (*)(void*);
		using GetAuthorFn = const std::string& (*)(void*);
		using GetWebsiteFn = const std::string& (*)(void*);
		using GetBaseDirFn = const std::filesystem::path& (*)(void*);
		using GetDependenciesFn = std::vector<std::string_view> (*)(void*);
		using FindResourceFn = std::optional<std::filesystem::path> (*)(void*, const std::filesystem::path&);

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
		const std::string& GetName() const { return plugin::GetName(plugin::handle); }
		const std::string& GetFullName() const { return plugin::GetFullName(plugin::handle); }
		const std::string& GetDescription() const { return plugin::GetDescription(plugin::handle); }
		const std::string& GetVersion() const { return plugin::GetVersion(plugin::handle); }
		const std::string& GetAuthor() const { return plugin::GetAuthor(plugin::handle); }
		const std::string& GetWebsite() const { return plugin::GetWebsite(plugin::handle); }
		const std::filesystem::path& GetBaseDir() const { return plugin::GetBaseDir(plugin::handle); }
		std::vector<std::string_view> GetDependencies() const { return plugin::GetDependencies(plugin::handle); }
		std::optional<std::filesystem::path> FindResource(const std::filesystem::path& path) const { return plugin::FindResource(plugin::handle, path); }

		virtual void OnPluginStart() {};
		virtual void OnPluginEnd() {};
	};

	IPluginEntry* GetPluginEntry();
}

#define EXPOSE_PLUGIN(plugin_api, interface_addr) namespace plugify { \
	GetMethodPtrFn GetMethodPtr{ nullptr }; \
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
	plugin_api int Plugify_Init(void** api, int version, void* handle) { \
		if (version < kApiVersion) { \
			return kApiVersion; \
		} \
        size_t i = 0; \
		GetMethodPtr = reinterpret_cast<GetMethodPtrFn>(api[i++]); \
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
	extern "C" \
	plugin_api void Plugify_PluginStart() { \
		GetPluginEntry()->OnPluginStart(); \
	} \
	extern "C" \
	plugin_api void Plugify_PluginEnd() { \
		GetPluginEntry()->OnPluginEnd(); \
	} \
	plugify::IPluginEntry* GetPluginEntry() { \
		return interface_addr; \
	} \
}

namespace plugify {
	struct Vector2 {
		float x{};
		float y{};
	};

	struct Vector3 {
		float x{};
		float y{};
		float z{};
	};

	struct Vector4 {
		float x{};
		float y{};
		float z{};
		float w{};
	};

	struct Matrix4x4 {
		float m[4][4]{};
	};
}