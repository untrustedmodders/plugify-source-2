#pragma once

#include <utils/assembly.h>

namespace cs2sdk::modules {
	class Module {
	public:
		Module(const char* path, const char* module);

		void* FindSignature(std::span<uint8_t> signature) const;
		void* FindInterface(const char* name) const;

		const Assembly& GetAssembly() const { return *m_assembly; }

	private:
		const char* m_pszModule;
		const char* m_pszPath;

		std::unique_ptr<Assembly> m_assembly;
	};
}

