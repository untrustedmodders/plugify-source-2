#pragma once

#include "KeyValues.h"

namespace cs2sdk {
	namespace modules {
		class CModule;
	}
	using ModuleRef = std::optional<std::reference_wrapper<modules::CModule>>;

	class CGameConfig {
	public:
		CGameConfig(std::string game, std::string path);
		~CGameConfig();

		bool Initialize();

		const std::string& GetPath() const;
		std::string_view GetLibrary(const std::string& name) const;
		std::string_view GetSignature(const std::string& name) const;
		std::string_view GetSymbol(const std::string& name) const;
		std::string_view GetPatch(const std::string& name) const;
		int GetOffset(const std::string& name) const;
		//void* GetAddress(const std::string& name, void* engine, void* server, char* error, int maxlen) const; // TODO: implement
		ModuleRef GetModule(const std::string& name) const;
		bool IsSymbol(const std::string& name) const;
		void* ResolveSignature(const std::string& name) const;

		static std::vector<uint8_t> HexToByte(std::string_view hexString);

	private:
		std::string m_szGameDir;
		std::string m_szPath;
		KeyValues m_pKeyValues;
		std::unordered_map<std::string, int> m_umOffsets;
		std::unordered_map<std::string, std::string> m_umSignatures;
		//std::unordered_map<std::string, void*> m_umAddresses;
		std::unordered_map<std::string, std::string> m_umLibraries;
		std::unordered_map<std::string, std::string> m_umPatches;
	};
}