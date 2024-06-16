// DynoHook
// Copyright (C) 2023-2024 qubka
// https://github.com/qubka/DynoHook/

#pragma once

#include <plugify/cpp_plugin.h>

namespace dyno
{
	class IHook;

	enum class CallbackType : bool
	{
		Pre, // callback will be executed before the original function
		Post // callback will be executed after the original function
	};

	enum class ReturnAction : int32_t
	{
		Ignored,  // handler didn't take any action
		Handled,  // we did something, but real function should still be called
		Override, // call real function, but use my return value
		Supercede // skip real function; use my return value
	};

	enum class DataType : uint8_t
	{
		Void,
		Bool,
		Int8,
		UInt8,
		Int16,
		UInt16,
		Int32,
		UInt32,
		Int64,
		UInt64,
		Float,
		Double,
		Pointer,
		String,
		WString,
	};

	enum class RegisterType : uint8_t
	{
		// no register at all.
		None,

		// ========================================================================
		// >> 8-bit General purpose registers
		// ========================================================================
		AL,
		CL,
		DL,
		BL,

		SPL,
		BPL,
		SIL,
		DIL,
		R8B,
		R9B,
		R10B,
		R11B,
		R12B,
		R13B,
		R14B,
		R15B,

		AH,
		CH,
		DH,
		BH,

		// ========================================================================
		// >> 16-bit General purpose registers
		// ========================================================================
		AX,
		CX,
		DX,
		BX,
		SP,
		BP,
		SI,
		DI,

		R8W,
		R9W,
		R10W,
		R11W,
		R12W,
		R13W,
		R14W,
		R15W,

		// ========================================================================
		// >> 32-bit General purpose registers
		// ========================================================================
		EAX,
		ECX,
		EDX,
		EBX,
		ESP,
		EBP,
		ESI,
		EDI,

		R8D,
		R9D,
		R10D,
		R11D,
		R12D,
		R13D,
		R14D,
		R15D,

		// ========================================================================
		// >> 64-bit General purpose registers
		// ========================================================================
		RAX,
		RCX,
		RDX,
		RBX,
		RSP,
		RBP,
		RSI,
		RDI,

		R8,
		R9,
		R10,
		R11,
		R12,
		R13,
		R14,
		R15,

		// ========================================================================
		// >> 64-bit MM (MMX) registers
		// ========================================================================
		MM0,
		MM1,
		MM2,
		MM3,
		MM4,
		MM5,
		MM6,
		MM7,

		// ========================================================================
		// >> 128-bit XMM registers
		// ========================================================================
		XMM0,
		XMM1,
		XMM2,
		XMM3,
		XMM4,
		XMM5,
		XMM6,
		XMM7,
		XMM8,
		XMM9,
		XMM10,
		XMM11,
		XMM12,
		XMM13,
		XMM14,
		XMM15,

		// ========================================================================
		// >> 256-bit YMM registers
		// ========================================================================
		YMM0,
		YMM1,
		YMM2,
		YMM3,
		YMM4,
		YMM5,
		YMM6,
		YMM7,
		YMM8,
		YMM9,
		YMM10,
		YMM11,
		YMM12,
		YMM13,
		YMM14,
		YMM15,

		// ========================================================================
		// >> 16-bit Segment registers
		// ========================================================================
		CS,
		SS,
		DS,
		ES,
		FS,
		GS,
	};

	class IHook;
	using CallbackHandler = ReturnAction (*)(CallbackType, IHook&);

	struct DataObject
	{
		DataType type;
		RegisterType reg;
		uint16_t size;

		DataObject(DataType type = DataType::Void, RegisterType reg = RegisterType::None, uint16_t size = 0) : type(type), reg(reg), size(size)
		{
		}
	};

	class CHook
	{
	private:
		CHook(IHook* pHook, void* pFunc, void* pClass, int index) : m_hook(pHook), m_func(pFunc), m_class(pClass), m_index(index)
		{
		}

	public:
		~CHook()
		{
			// detour
			if (!m_class)
			{
				using UnhookDetourFn = bool (*)(void*);
				static auto func = reinterpret_cast<UnhookDetourFn>(plugify::GetMethodPtr("dynohook.UnhookDetour"));
				func(m_func);
			}
			else if (m_func)
			{
				using UnhookVirtualByFuncFn = bool (*)(void*, void*);
				static auto func = reinterpret_cast<UnhookVirtualByFuncFn>(plugify::GetMethodPtr("dynohook.UnhookVirtualByFunc"));
				func(m_class, m_func);
			}
			else
			{
				using UnhookVirtualFn = bool (*)(void*, int);
				static auto func = reinterpret_cast<UnhookVirtualFn>(plugify::GetMethodPtr("dynohook.UnhookVirtual"));
				func(m_class, m_index);
			}
		}

		static std::unique_ptr<CHook> CreateDetourHook(void* pFunc, const std::vector<DataObject>& arguments, DataObject returnType)
		{
			using HookDetourFn = IHook* (*)(void*, const std::vector<DataObject>&, DataObject);
			static auto func = reinterpret_cast<HookDetourFn>(plugify::GetMethodPtr("dynohook.HookDetour"));
			IHook* pHook = func(pFunc, arguments, returnType);
			if (pHook == nullptr)
				return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, nullptr, -1));
		}

		static std::unique_ptr<CHook> CreateVirtualHook(void* pClass, int index, const std::vector<DataObject>& arguments, DataObject returnType)
		{
			using HookVirtualFn = IHook* (*)(void*, int, const std::vector<DataObject>&, DataObject);
			static auto func = reinterpret_cast<HookVirtualFn>(plugify::GetMethodPtr("dynohook.HookVirtual"));
			IHook* pHook = func(pClass, index, arguments, returnType);
			if (pHook == nullptr)
				return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, nullptr, pClass, index));
		}

		static std::unique_ptr<CHook> CreateHookVirtualByFunc(void* pClass, void* pFunc, const std::vector<DataObject>& arguments, DataObject returnType)
		{
			using HookVirtualByFuncFn = IHook* (*)(void*, void*, const std::vector<DataObject>&, DataObject);
			static auto func = reinterpret_cast<HookVirtualByFuncFn>(plugify::GetMethodPtr("dynohook.HookVirtualByFunc"));
			IHook* pHook = func(pClass, pFunc, arguments, returnType);
			if (pHook == nullptr)
				return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, pClass, -1));
		}

		bool AddCallback(CallbackType type, CallbackHandler handler) const
		{
			using AddCallbackFn = bool (*)(IHook*, bool, CallbackHandler);
			static auto func = reinterpret_cast<AddCallbackFn>(plugify::GetMethodPtr("dynohook.AddCallback"));
			return func(m_hook, static_cast<bool>(type), handler);
		}

		bool RemoveCallback(CallbackType type, CallbackHandler handler) const
		{
			using RemoveCallbackFn = bool (*)(IHook*, bool, CallbackHandler);
			static auto func = reinterpret_cast<RemoveCallbackFn>(plugify::GetMethodPtr("dynohook.RemoveCallback"));
			return func(m_hook, static_cast<bool>(type), handler);
		}

		bool IsCallbackRegistered(bool type, CallbackHandler handler) const
		{
			using IsCallbackRegisteredFn = bool (*)(IHook*, bool, CallbackHandler);
			static auto func = reinterpret_cast<IsCallbackRegisteredFn>(plugify::GetMethodPtr("dynohook.IsCallbackRegistered"));
			return func(m_hook, type, handler);
		}

		bool AreCallbacksRegistered() const
		{
			using AreCallbacksRegisteredFn = bool (*)(IHook*);
			static auto func = reinterpret_cast<AreCallbacksRegisteredFn>(plugify::GetMethodPtr("dynohook.AreCallbacksRegistered"));
			return func(m_hook);
		}

	private:
		IHook* m_hook;
		void* m_func;
		void* m_class;
		int m_index;
	};

	template <class T> requires(!std::is_reference_v<T>)
	inline T GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = T (*)(IHook*, size_t);
		static GetArgumentFn func;
		if constexpr (std::is_same_v<T, bool>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentBool"));
		else if constexpr (std::is_same_v<T, int8_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt8"));
		else if constexpr (std::is_same_v<T, uint8_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentUInt8"));
		else if constexpr (std::is_same_v<T, int16_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt16"));
		else if constexpr (std::is_same_v<T, uint16_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentUInt16"));
		else if constexpr (std::is_same_v<T, int32_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt32"));
		else if constexpr (std::is_same_v<T, uint32_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentUInt32"));
		else if constexpr (std::is_same_v<T, int64_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt64"));
		else if constexpr (std::is_same_v<T, uint64_t>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentUInt64"));
		else if constexpr (std::is_same_v<T, float>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentFloat"));
		else if constexpr (std::is_same_v<T, double>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentDouble"));
		else if constexpr (std::is_same_v<T, const char*>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentString"));
		else if constexpr (std::is_same_v<T, const wchar_t*>)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentWString"));
		else if constexpr (std::is_pointer<T>::value)
			func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentPointer"));
		else
		{
			if (sizeof(T) > sizeof(int64_t))
			{
				static_assert("Unsupported type");
			}
			else if constexpr (sizeof(T) > sizeof(int32_t))
			{
				func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt64"));
			}
			else if constexpr (sizeof(T) > sizeof(int16_t))
			{
				func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt32"));
			}
			else if constexpr (sizeof(T) > sizeof(int8_t))
			{
				func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt16"));
			}
			else if constexpr (sizeof(T) > sizeof(bool))
			{
				func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentInt8"));
			}
			else
			{
				func = reinterpret_cast<GetArgumentFn>(plugify::GetMethodPtr("dynohook.GetArgumentBool"));
			}
		}
		return func(&hook, index);
	}

	template <class T> requires(!std::is_reference_v<T>)
	inline void SetArgument(IHook& hook, size_t index, T value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, T);
		static SetArgumentFn func;
		if constexpr (std::is_same_v<T, bool>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentBool"));
		else if constexpr (std::is_same_v<T, int8_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt8"));
		else if constexpr (std::is_same_v<T, uint8_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentUInt8"));
		else if constexpr (std::is_same_v<T, int16_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt16"));
		else if constexpr (std::is_same_v<T, uint16_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentUInt16"));
		else if constexpr (std::is_same_v<T, int32_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt32"));
		else if constexpr (std::is_same_v<T, uint32_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentUInt32"));
		else if constexpr (std::is_same_v<T, int64_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt64"));
		else if constexpr (std::is_same_v<T, uint64_t>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentUInt64"));
		else if constexpr (std::is_same_v<T, float>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentFloat"));
		else if constexpr (std::is_same_v<T, double>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentDouble"));
		else if constexpr (std::is_same_v<T, const char*>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentString"));
		else if constexpr (std::is_same_v<T, const wchar_t*>)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentWString"));
		else if constexpr (std::is_pointer<T>::value)
			func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentPointer"));
		else
		{
			if (sizeof(T) > sizeof(int64_t))
			{
				static_assert("Unsupported type");
			}
			else if constexpr (sizeof(T) > sizeof(int32_t))
			{
				func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt64"));
			}
			else if constexpr (sizeof(T) > sizeof(int16_t))
			{
				func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt32"));
			}
			else if constexpr (sizeof(T) > sizeof(int8_t))
			{
				func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt16"));
			}
			else if constexpr (sizeof(T) > sizeof(bool))
			{
				func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentInt8"));
			}
			else
			{
				func = reinterpret_cast<SetArgumentFn>(plugify::GetMethodPtr("dynohook.SetArgumentBool"));
			}
		}
		func(&hook, index, value);
	}

	template <class T> requires(!std::is_reference_v<T>)
	inline T GetReturn(IHook& hook)
	{
		using GetReturnFn = T (*)(IHook*);
		static GetReturnFn func;
		if constexpr (std::is_same_v<T, bool>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnBool"));
		else if constexpr (std::is_same_v<T, int8_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt8"));
		else if constexpr (std::is_same_v<T, uint8_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnUInt8"));
		else if constexpr (std::is_same_v<T, int16_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt16"));
		else if constexpr (std::is_same_v<T, uint16_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnUInt16"));
		else if constexpr (std::is_same_v<T, int32_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt32"));
		else if constexpr (std::is_same_v<T, uint32_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnUInt32"));
		else if constexpr (std::is_same_v<T, int64_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt64"));
		else if constexpr (std::is_same_v<T, uint64_t>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnUInt64"));
		else if constexpr (std::is_same_v<T, float>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnFloat"));
		else if constexpr (std::is_same_v<T, double>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnDouble"));
		else if constexpr (std::is_same_v<T, const char*>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnString"));
		else if constexpr (std::is_same_v<T, const wchar_t*>)
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnWString"));
		else if constexpr (std::is_pointer<T>::value )
			func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnPointer"));
		else
		{
			/*if (sizeof(T) > sizeof(int64_t))
			{
				static_assert("Unsupported type");
			}
			else if constexpr (sizeof(T) > sizeof(int32_t))
			{
				func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt64"));
			}
			else if constexpr (sizeof(T) > sizeof(int16_t))
			{
				func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt32"));
			}
			else if constexpr (sizeof(T) > sizeof(int8_t))
			{
				func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt16"));
			}
			else if constexpr (sizeof(T) > sizeof(bool))
			{
				func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnInt8"));
			}
			else
			{
				func = reinterpret_cast<GetReturnFn>(plugify::GetMethodPtr("dynohook.GetReturnBool"));
			}*/
			static_assert("Unsupported type");
		}
		return func(&hook);
	}

	template <class T> requires(!std::is_reference_v<T>)
	inline void SetReturn(IHook& hook, T value)
	{
		using SetReturnFn = void (*)(IHook*, T);
		static SetReturnFn func;
		if constexpr (std::is_same_v<T, bool>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnBool"));
		else if constexpr (std::is_same_v<T, int8_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt8"));
		else if constexpr (std::is_same_v<T, uint8_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnUInt8"));
		else if constexpr (std::is_same_v<T, int16_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt16"));
		else if constexpr (std::is_same_v<T, uint16_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnUInt16"));
		else if constexpr (std::is_same_v<T, int32_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt32"));
		else if constexpr (std::is_same_v<T, uint32_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnUInt32"));
		else if constexpr (std::is_same_v<T, int64_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt64"));
		else if constexpr (std::is_same_v<T, uint64_t>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnUInt64"));
		else if constexpr (std::is_same_v<T, float>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnFloat"));
		else if constexpr (std::is_same_v<T, double>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnDouble"));
		else if constexpr (std::is_same_v<T, const char*>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnString"));
		else if constexpr (std::is_same_v<T, const wchar_t*>)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnWString"));
		else if constexpr (std::is_pointer<T>::value)
			func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnPointer"));
		else
		{
			/*if (sizeof(T) > sizeof(int64_t))
			{
				static_assert("Unsupported type");
			}
			else if constexpr (sizeof(T) > sizeof(int32_t))
			{
				func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt64"));
			}
			else if constexpr (sizeof(T) > sizeof(int16_t))
			{
				func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt32"));
			}
			else if constexpr (sizeof(T) > sizeof(int8_t))
			{
				func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt16"));
			}
			else if constexpr (sizeof(T) > sizeof(bool))
			{
				func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnInt8"));
			}
			else
			{
				func = reinterpret_cast<SetReturnFn>(plugify::GetMethodPtr("dynohook.SetReturnBool"));
			}*/
			static_assert("Unsupported type");
		}
		func(&hook, value);
	}

	namespace details
	{
		template <typename T>
		constexpr DataType GetType()
		{
			if constexpr (std::is_same_v<T, void>)
				return DataType::Void;
			else if constexpr (std::is_same_v<T, bool>)
				return DataType::Bool;
			else if constexpr (std::is_same_v<T, int8_t>)
				return DataType::Int8;
			else if constexpr (std::is_same_v<T, uint8_t>)
				return DataType::UInt8;
			else if constexpr (std::is_same_v<T, int16_t>)
				return DataType::Int16;
			else if constexpr (std::is_same_v<T, uint16_t>)
				return DataType::UInt16;
			else if constexpr (std::is_same_v<T, int32_t>)
				return DataType::Int32;
			else if constexpr (std::is_same_v<T, uint32_t>)
				return DataType::UInt32;
			else if constexpr (std::is_same_v<T, int64_t>)
				return DataType::Int64;
			else if constexpr (std::is_same_v<T, uint64_t> || std::is_same_v<T, unsigned long>)
				return DataType::UInt64;
			else if constexpr (std::is_same_v<T, float>)
				return DataType::Float;
			else if constexpr (std::is_same_v<T, double>)
				return DataType::Double;
			else if constexpr (std::is_same_v<T, const char*>)
				return DataType::String;
			else if constexpr (std::is_same_v<T, const wchar_t*>)
				return DataType::WString;
			else if constexpr (std::is_pointer<T>::value || std::is_reference_v<T>)
				return DataType::Pointer;
			else
			{
				if (sizeof(T) > sizeof(int64_t))
				{
					return DataType::Pointer;
				}
				else if constexpr (sizeof(T) > sizeof(int32_t))
				{
					return DataType::Int64;
				}
				else if constexpr (sizeof(T) > sizeof(int16_t))
				{
					return DataType::Int32;
				}
				else if constexpr (sizeof(T) > sizeof(int8_t))
				{
					return DataType::Int16;
				}
				else if constexpr (sizeof(T) > sizeof(bool))
				{
					return DataType::Int8;
				}
				else
				{
					return DataType::Bool;
				}
			}
		}

		template<class>
		class function_traits;

		template <typename Function>
		struct function_traits;

		template <typename Ret, typename... Args>
		struct function_traits<Ret (*)(Args...)>
		{
			static constexpr size_t arity = sizeof...(Args);

			static std::array<DataObject, arity> args()
			{
				return {GetType<Args>()...};
			}

			static DataObject ret()
			{
				return GetType<Ret>();
			}
		};

		template <typename Class, typename Ret, typename... Args>
		struct function_traits<Ret (Class::*)(Args...)>
		{
			static constexpr size_t arity = sizeof...(Args);

			static std::array<DataObject, arity + 1> args()
			{
				std::array<DataObject, arity> originalArgs = {GetType<Args>()...};
				std::array<DataObject, arity + 1> result;
				result[0] = DataType::Pointer;
				std::copy(originalArgs.begin(), originalArgs.end(), result.begin() + 1);
				return result;
			}

			static DataObject ret()
			{
				return GetType<Ret>();
			}
		};

		template <typename Class, typename Ret, typename... Args>
		class function_traits<Ret (Class::*)(Args...) const> : public function_traits<Ret (Class::*)(Args...)> {};
		template <typename Class, typename Ret, typename... Args>
		class function_traits<Ret (Class::*)(Args...) volatile> : public function_traits<Ret (Class::*)(Args...)> {};
		template <typename Class, typename Ret, typename... Args>
		class function_traits<Ret (Class::*)(Args...) const volatile> : public function_traits<Ret (Class::*)(Args...)> {};

		template <typename Ret, typename... Args>
		struct function_traits<Ret(Args...)>
		{
			static constexpr size_t arity = sizeof...(Args);

			static std::array<DataObject, arity> args()
			{
				return {GetType<Args>()...};
			}

			static DataObject ret()
			{
				return GetType<Ret>();
			}
		};

		template<class Ret, class... ArgTypes>
		class function_traits<Ret(ArgTypes...) const> : public function_traits<Ret(ArgTypes...)> {};
		template<class Ret, class... ArgTypes>
		class function_traits<Ret(ArgTypes...) volatile> : public function_traits<Ret(ArgTypes...)> {};
		template<class Ret, class... ArgTypes>
		class function_traits<Ret(ArgTypes...) const volatile> : public function_traits<Ret(ArgTypes...)> {};

	} // namespace details
} // namespace dyno