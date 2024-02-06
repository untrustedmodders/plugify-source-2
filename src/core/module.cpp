#include "module.h"
#include "fwd.h"

using namespace modules;

CModule::CModule(const char* path, const char* module) : m_pszModule{module}, m_pszPath{path} {
	char szModule[260];
	V_snprintf(szModule, sizeof(szModule), "%s%s" BINARY_MODULE_PREFIX "%s" BINARY_MODULE_SUFFIX, Plat_GetGameDirectory(), m_pszPath, m_pszModule);

	m_assembly = CAssembly::FindModule(szModule);
	if (!m_assembly) {
		Log_Error(LOG_GENERAL, "Could not find: %s\n", szModule);
	}
}

void* CModule::FindSignature(std::span<uint8_t> signature) const {
	return m_assembly->FindSignature(signature);
}

void* CModule::FindInterface(const char* name) const {
	auto fn = m_assembly->GetFunction<CreateInterfaceFn>("CreateInterface");
	if (!fn) {
		Log_Error(LOG_GENERAL, "Could not find CreateInterface in %s\n", m_pszModule);
	}

	void* pInterface = fn(name, nullptr);
	if (!pInterface) {
		Log_Error(LOG_GENERAL, "Could not find interface: %s in %s\n", name, m_pszModule);
	}

	return pInterface;
}