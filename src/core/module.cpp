#include "module.h"

using namespace cs2sdk::modules;

Module::Module(const char* path, const char* module) : m_pszModule{module}, m_pszPath{path} {
	char szModule[MAX_PATH];
	V_snprintf(szModule, MAX_PATH, "%s%s" BINARY_MODULE_PREFIX "%s" BINARY_MODULE_SUFFIX, Plat_GetGameDirectory(), m_pszPath, m_pszModule);

	m_assembly = Assembly::LoadFromPath(szModule);
	if (!m_assembly) {
		Error("Could not find %s\n", szModule);
	}
}

void* Module::FindSignature(std::span<uint8_t> signature) const {
	if (signature.empty())
		return nullptr;

	size_t iSigLength = signature.size() - 1; // not count null char

	uint8_t* pMemory;
	void* pReturnAddr = nullptr;

	pMemory = (uint8_t*)m_assembly->GetBase();

	for (size_t i = 0; i < m_assembly->GetSize(); ++i) {
		size_t matches = 0;
		while (*(pMemory + i + matches) == signature[matches] || signature[matches] == '\x2A') {
			matches++;
			if (matches == iSigLength)
				pReturnAddr = (void*)(pMemory + i);
		}
	}

	return pReturnAddr;
}

void* Module::FindInterface(const char* name) const {
	auto fn = m_assembly->GetFunction<CreateInterfaceFn>("CreateInterface");
	if (!fn)
		Error("Could not find CreateInterface in %s\n", m_pszModule);

	void* pInterface = fn(name, nullptr);
	if (!pInterface)
		Error("Could not find %s in %s\n", name, m_pszModule);

	return pInterface;
}