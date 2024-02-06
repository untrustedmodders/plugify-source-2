#pragma once

#include <utils/assembly.h>

namespace modules {
	class CModule {
	public:
		CModule(const char* path, const char* module);

		void* FindSignature(std::span<uint8_t> signature) const;
		void* FindInterface(const char* name) const;

		const CAssembly& GetAssembly() const { return *m_assembly; }

	private:
		const char* m_pszModule;
		const char* m_pszPath;

		std::unique_ptr<CAssembly> m_assembly;
	};
}

