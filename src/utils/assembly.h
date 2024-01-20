#pragma once

namespace cs2sdk {
    class Assembly {
    public:
        static std::unique_ptr<Assembly> LoadFromPath(const std::string& assemblyPath);
        static std::string GetError();

        ~Assembly();

        void* GetFunction(const char* functionName) const;
        template<class _Fn> requires(std::is_pointer_v<_Fn> && std::is_function_v<std::remove_pointer_t<_Fn>>)
        _Fn GetFunction(const char* functionName) const {
            return reinterpret_cast<_Fn>(GetFunction(functionName));
        }

		void* GetHandle() const {
			return m_handle;
		}

		void* GetBase() const {
			return m_base;
		}

		size_t GetSize() const {
			return m_size;
		}

    private:
        explicit Assembly(void* handle, void* base, size_t size);

    private:
        void* m_handle{ nullptr };
		void* m_base{ nullptr };
		size_t m_size{};
    };
}
