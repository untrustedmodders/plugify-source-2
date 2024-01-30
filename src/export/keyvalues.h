#pragma once

#include <tier1/KeyValues.h>

extern "C"
PLUGIN_API void KeyValues_DeleteKeyValues(KeyValues* pKeyValues) {
	delete pKeyValues;
}

extern "C"
PLUGIN_API KeyValues* KeyValues_CreateKeyValues1(const char* setName) {
	return new KeyValues(setName);
}

extern "C"
PLUGIN_API KeyValues* KeyValues_CreateKeyValues2(const char* setName, const char* firstKey, const char* firstValue) {
	return new KeyValues(setName, firstKey, firstValue);
}

extern "C"
PLUGIN_API KeyValues* KeyValues_CreateKeyValues3(const char* setName, const char* firstKey, int firstValue) {
	return new KeyValues(setName, firstKey, firstValue);
}

extern "C"
PLUGIN_API KeyValues* KeyValues_CreateKeyValues4(const char* setName, const char* firstKey, const char* firstValue, const char* secondKey, const char* secondValue) {
	return new KeyValues(setName, firstKey, firstValue, secondKey, secondValue);
}

extern "C"
PLUGIN_API KeyValues* KeyValues_MakeCopy(KeyValues* pKeys) {
	return pKeys->MakeCopy();
}

extern "C"
PLUGIN_API KeyValues* KeyValues_FromFile(const char* szFile, bool bUsesEscapeSequences = false) {
	KeyValues* pKeyValues = new KeyValues("");
	pKeyValues->UsesEscapeSequences(bUsesEscapeSequences);
	if (!pKeyValues->LoadFromFile(g_pFullFileSystem, szFile, nullptr)) {
		delete pKeyValues;
		Log_Error(LOG_GENERAL, "Failed to load from file.");
		return nullptr;
	}

	return pKeyValues;
}

extern "C"
PLUGIN_API KeyValues* KeyValues_FromBuffer(const char* buffer) {
	KeyValues* pKeyValues = new KeyValues("");
	if (!pKeyValues->LoadFromBuffer("", buffer, g_pFullFileSystem)) {
		delete pKeyValues;
		Log_Error(LOG_GENERAL, "Failed to load from buffer.");
		return nullptr;
	}
	return pKeyValues;
}

extern "C"
PLUGIN_API bool KeyValues_FromBufferInPlace(KeyValues* pKeyValues, const char* buffer) {
	return pKeyValues->LoadFromBuffer(pKeyValues->GetName(), buffer, g_pFullFileSystem);
}

extern "C"
PLUGIN_API bool KeyValues_SaveToFile(KeyValues* pKeyValues, const char* szFile) {
	return pKeyValues->SaveToFile(g_pFullFileSystem, szFile);
}

extern "C"
PLUGIN_API void KeyValues_SetWString(KeyValues* pKeyValues, const char* szKeyName, const wchar_t* value) {
	pKeyValues->SetWString(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetString(KeyValues* pKeyValues, const char* szKeyName, const char* value) {
	pKeyValues->SetString(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetInt(KeyValues* pKeyValues, const char* szKeyName, int value) {
	pKeyValues->SetInt(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetUint64(KeyValues* pKeyValues, const char* szKeyName, uint64 value) {
	pKeyValues->SetUint64(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetFloat(KeyValues* pKeyValues, const char* szKeyName, float value) {
	pKeyValues->SetFloat(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetPtr(KeyValues* pKeyValues, const char* szKeyName, void* value) {
	pKeyValues->SetPtr(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetColor(KeyValues* pKeyValues, const char* szKeyName, Color value) {
	pKeyValues->SetColor(szKeyName, value);
}

extern "C"
PLUGIN_API void KeyValues_SetBool(KeyValues* pKeyValues, const char* szKeyName, bool value) {
	pKeyValues->SetInt(szKeyName, value ? 1 : 0);
}

extern "C"
PLUGIN_API int KeyValues_GetInt(KeyValues* pKeyValues, const char* szKeyName = NULL, int defaultValue = 0) {
	return pKeyValues->GetInt(szKeyName, defaultValue);
}

extern "C"
PLUGIN_API uint64 KeyValues_GetUint64(KeyValues* pKeyValues, const char* szKeyName = NULL, uint64 defaultValue = 0) {
	return pKeyValues->GetUint64(szKeyName, defaultValue);
}

extern "C"
PLUGIN_API float KeyValues_GetFloat(KeyValues* pKeyValues, const char* szKeyName = NULL, float defaultValue = 0.0f) {
	return pKeyValues->GetFloat(szKeyName, defaultValue);
}

extern "C"
PLUGIN_API const char* KeyValues_GetString(KeyValues* pKeyValues, const char* szKeyName, const char* defaultValue = "", char* pszOut = NULL, size_t maxlen = 0) {
	return pKeyValues->GetString(szKeyName, defaultValue, pszOut, maxlen);
}

extern "C"
PLUGIN_API const wchar_t* KeyValues_GetWString(KeyValues* pKeyValues, const char* szKeyName = NULL, const wchar_t* defaultValue = L"", wchar_t* pszOut = NULL, size_t maxlen = 0) {
	return pKeyValues->GetWString(szKeyName, defaultValue, pszOut, maxlen);
}

extern "C"
PLUGIN_API void* KeyValues_GetPtr(KeyValues* pKeyValues, const char* szKeyName = NULL, void* defaultValue = (void*) 0) {
	return pKeyValues->GetPtr(szKeyName, defaultValue);
}

extern "C"
PLUGIN_API color32 KeyValues_GetColor(KeyValues* pKeyValues, const char* szKeyName = NULL, const Color& defaultColor = Color(0, 0, 0, 0)) {
	return pKeyValues->GetColor(szKeyName, defaultColor).ToColor32();
}

extern "C"
PLUGIN_API bool KeyValues_GetBool(KeyValues* pKeyValues, const char* szKeyName = NULL, bool defaultValue = false) {
	return pKeyValues->GetInt(szKeyName, defaultValue ? 1 : 0);
}

extern "C"
PLUGIN_API bool KeyValues_IsEmpty(KeyValues* pKeyValues, const char* szKeyName = NULL) {
	return pKeyValues->IsEmpty(szKeyName);
}