#pragma once

namespace cs2sdk {
    class CAssembly {
    public:
        static std::unique_ptr<CAssembly> FindModule(const std::string& assemblyPath);
        static std::unique_ptr<CAssembly> LoadFromPath(const std::string& assemblyPath);
        static std::string GetError();

        ~CAssembly();

        void* GetFunction(const char* functionName) const;
        template<class _Fn> requires(std::is_pointer_v<_Fn> && std::is_function_v<std::remove_pointer_t<_Fn>>)
        _Fn GetFunction(const char* functionName) const {
            return reinterpret_cast<_Fn>(GetFunction(functionName));
        }

		void* FindSignature(std::span<uint8_t> signature) const;

		void* GetBase() const {
			return m_base;
		}

		size_t GetSize() const {
			return m_size;
		}

    private:
        explicit CAssembly(void* handle, void* base, size_t size, bool owner);

		static std::unique_ptr<CAssembly> Create(void* handle, bool owner);

    private:
        void* m_handle{ nullptr };
		void* m_base{ nullptr };
		size_t m_size{};
		[[maybe_unused]] bool m_owner{};
    };
}
