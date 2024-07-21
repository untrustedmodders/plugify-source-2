#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <filesystem>
#include <vector>
#include <span>

#undef FindResource

namespace plugify {
	constexpr int kApiVersion = 1;

	using InitFunc = int (*)(std::span<void*>, int, void*);
	using StartFunc = void (*)();
	using EndFunc = void (*)();

	using GetMethodPtrFn = void* (*)(std::string_view);
	using GetMethodPtr2Fn = void (*)(std::string_view, void**);
	using GetBaseDirFn = const std::filesystem::path& (*)();
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
		using GetBaseDirFn = const std::filesystem::path& (*)(void*);
		using GetDependenciesFn = std::vector<std::string_view> (*)(void*);
		using FindResourceFn = std::optional<std::filesystem::path> (*)(void*, const std::filesystem::path&);
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
	plugin_api int Plugify_Init(std::span<void*> api, int version, void* handle) { \
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

		bool operator==(const Vector2&) const = default;
	};

	struct Vector3 {
		float x{};
		float y{};
		float z{};

		bool operator==(const Vector3&) const = default;
	};

	struct Vector4 {
		float x{};
		float y{};
		float z{};
		float w{};

		bool operator==(const Vector4&) const = default;
	};

	struct Matrix4x4 {
		float m[4][4]{};

		bool operator==(const Matrix4x4&) const = default;
	};
}