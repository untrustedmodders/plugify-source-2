#include "assembly.h"

#if CS2SDK_PLATFORM_WINDOWS
#include <windows.h>
#include <psapi.h>
#elif CS2SDK_PLATFORM_LINUX
#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#elif CS2SDK_PLATFORM_APPLE
#include <dlfcn.h>
#else
#error "Platform not supported"
#endif

namespace cs2sdk {
    thread_local static std::string lastError;

#if CS2SDK_PLATFORM_LINUX
#define PAGE_SIZE 4096
#define PAGE_ALIGN_UP(x) ((x + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// https://github.com/alliedmodders/sourcemod/blob/master/core/logic/MemoryUtils.cpp#L502-L587
	int GetModuleInformation(void* hModule, void** base, size_t* length) {
		struct link_map* dlmap = (struct link_map *)hModule;
		Dl_info info;
		Elf64_Ehdr* file;
		Elf64_Phdr* phdr;
		uint16_t phdrCount;

		if (!dladdr((void *)dlmap->l_addr, &info)) {
			return 1;
		}

		if (!info.dli_fbase || !info.dli_fname) {
			return 2;
		}

		/* This is for our insane sanity checks :o */
		uintptr_t baseAddr = reinterpret_cast<uintptr_t>(info.dli_fbase);
		file = reinterpret_cast<Elf64_Ehdr *>(baseAddr);

		/* Check ELF magic */
		if (memcmp(ELFMAG, file->e_ident, SELFMAG) != 0) {
			return 3;
		}

		/* Check ELF version */
		if (file->e_ident[EI_VERSION] != EV_CURRENT) {
			return 4;
		}

		/* Check ELF endianness */
		if (file->e_ident[EI_DATA] != ELFDATA2LSB) {
			return 5;
		}

		/* Check ELF architecture */
		if (file->e_ident[EI_CLASS] != ELFCLASS64 || file->e_machine != EM_X86_64) {
			return 6;
		}

		/* For our purposes, this must be a dynamic library/shared object */
		if (file->e_type != ET_DYN) {
			return 7;
		}

		phdrCount = file->e_phnum;
		phdr = reinterpret_cast<Elf64_Phdr *>(baseAddr + file->e_phoff);

		for (uint16_t i = 0; i < phdrCount; i++) {
			Elf64_Phdr &hdr = phdr[i];

			/* We only really care about the segment with executable code */
			if (hdr.p_type == PT_LOAD && hdr.p_flags == (PF_X | PF_R)) {
				/* From glibc, elf/dl-load.c:
				 * c->mapend = ((ph->p_vaddr + ph->p_filesz + GLRO(dl_pagesize) - 1)
				 *             & ~(GLRO(dl_pagesize) - 1));
				 *
				 * In glibc, the segment file size is aligned up to the nearest page size and
				 * added to the virtual address of the segment. We just want the size here.
				 */
				*length = PAGE_ALIGN_UP(hdr.p_filesz);
				*base = (void *)(baseAddr + hdr.p_paddr);

				break;
			}
		}
		return 0;
	}
#elif CS2SDK_PLATFORM_WINDOWS
	bool UTF8StringToWideString(std::wstring& dest, std::string_view str) {
		int wlen = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), nullptr, 0);
		if (wlen < 0)
			return false;

		dest.resize(static_cast<size_t>(wlen));
		if (wlen > 0 && MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.length()), dest.data(), wlen) < 0)
			return false;

		return true;
	}
#endif

	std::unique_ptr<CAssembly> CAssembly::Create(void* handle, bool owner) {
		if (handle) {
			void* base = nullptr;
			size_t size = 0;
#ifdef CS2SDK_PLATFORM_WINDOWS
			MODULEINFO m_hModuleInfo;
			GetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(handle), &m_hModuleInfo, sizeof(m_hModuleInfo));

			base = m_hModuleInfo.lpBaseOfDll;
			size = m_hModuleInfo.SizeOfImage;
#elif CS2SDK_PLATFORM_LINUX
			GetModuleInformation(handle, &base, &size);
#endif
			if (base && size)
				return std::unique_ptr<CAssembly>(new CAssembly(handle, base, size, owner));
		}

#if CS2SDK_PLATFORM_WINDOWS
		uint32_t errorCode = GetLastError();
		if (errorCode != 0) {
			LPSTR messageBuffer = nullptr;
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
										 NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
			lastError = std::string(messageBuffer, size);
			LocalFree(messageBuffer);
		}
#elif CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
		lastError = dlerror();
#endif
		return nullptr;
	}

	std::unique_ptr<CAssembly> CAssembly::FindModule(const std::string& assemblyPath) {
#if CS2SDK_PLATFORM_WINDOWS
		std::wstring path;
		UTF8StringToWideString(path, assemblyPath);
		std::replace(path.begin(), path.end(), L'/', L'\\');
		void* handle = static_cast<void*>(GetModuleHandleW(path.c_str()));
#elif CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
		void* handle = dlopen(assemblyPath.c_str(), RTLD_NOLOAD);
#else
		void* handle = nullptr;
#endif
		return Create(handle, false);
	}

    std::unique_ptr<CAssembly> CAssembly::LoadFromPath(const std::string& assemblyPath) {
#if CS2SDK_PLATFORM_WINDOWS
		std::wstring path;
		UTF8StringToWideString(path, assemblyPath);
		std::replace(path.begin(), path.end(), L'/', L'\\');
        void* handle = static_cast<void*>(LoadLibraryW(path.c_str()));
#elif CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
        void* handle = dlopen(assemblyPath.c_str(), RTLD_LAZY);
#else
		void* handle = nullptr;
#endif
		return Create(handle, true);
    }

    std::string CAssembly::GetError() {
        return lastError;
    }

    CAssembly::CAssembly(void* handle, void* base, size_t size, bool owner) : m_handle{handle}, m_base{base}, m_size{size}, m_owner{owner} {
    }

    CAssembly::~CAssembly() {
#if CS2SDK_PLATFORM_WINDOWS
		if (m_owner)
			FreeLibrary(static_cast<HMODULE>(m_handle));
#elif CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
		dlclose(m_handle);
#endif
    }

    void* CAssembly::GetFunction(const char* functionName) const {
#if CS2SDK_PLATFORM_WINDOWS
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_handle), functionName));
#elif CS2SDK_PLATFORM_LINUX || CS2SDK_PLATFORM_APPLE
        return dlsym(m_handle, functionName);
#else
		return nullptr;
#endif
    }

	void* CAssembly::FindSignature(std::span<uint8_t> signature) const {
		if (signature.empty())
			return nullptr;

		size_t iSigLength = signature.size() - 1; // not count null char

		uint8_t* pMemory;
		void* pReturnAddr = nullptr;

		pMemory = (uint8_t*) m_base;

		for (size_t i = 0; i < m_size; ++i) {
			size_t matches = 0;
			while (*(pMemory + i + matches) == signature[matches] || signature[matches] == '\x2A') {
				matches++;
				if (matches == iSigLength)
					pReturnAddr = (void*)(pMemory + i);
			}
		}

		return pReturnAddr;
	}
}
