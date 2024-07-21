// dynohook
// Copyright (C) 2023-2024 qubka
// https://github.com/qubka/dynohook/

#pragma once

#include "cpp_plugin.h"

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
				static UnhookDetourFn func = nullptr;
				if (func == nullptr) plugify::GetMethodPtr2("dynhook.UnhookDetour", reinterpret_cast<void**>(&func));
				func(m_func);
			}
			else if (m_func)
			{
				using UnhookVirtualByFuncFn = bool (*)(void*, void*);
				static UnhookVirtualByFuncFn func = nullptr;
				if (func == nullptr) plugify::GetMethodPtr2("dynhook.UnhookVirtualByFunc", reinterpret_cast<void**>(&func));
				func(m_class, m_func);
			}
			else
			{
				using UnhookVirtualFn = bool (*)(void*, int);
				static UnhookVirtualFn func = nullptr;
				if (func == nullptr) plugify::GetMethodPtr2("dynhook.UnhookVirtual", reinterpret_cast<void**>(&func));
				func(m_class, m_index);
			}
		}

		static std::unique_ptr<CHook> CreateDetourHook(void* pFunc, const std::vector<DataObject>& arguments, DataObject returnType)
		{
			using HookDetourFn = IHook* (*)(void*, const std::vector<DataObject>&, DataObject);
			static HookDetourFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.HookDetour", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pFunc, arguments, returnType);
			if (pHook == nullptr) 	return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, nullptr, -1));
		}

		static std::unique_ptr<CHook> CreateVirtualHook(void* pClass, int index, const std::vector<DataObject>& arguments, DataObject returnType)
		{
			using HookVirtualFn = IHook* (*)(void*, int, const std::vector<DataObject>&, DataObject);
			static HookVirtualFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.HookVirtual", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pClass, index, arguments, returnType);
			if (pHook == nullptr) 	return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, nullptr, pClass, index));
		}

		static std::unique_ptr<CHook> CreateHookVirtualByFunc(void* pClass, void* pFunc, const std::vector<DataObject>& arguments, DataObject returnType)
		{
			using HookVirtualByFuncFn = IHook* (*)(void*, void*, const std::vector<DataObject>&, DataObject);
			static HookVirtualByFuncFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.HookVirtualByFunc", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pClass, pFunc, arguments, returnType);
			if (pHook == nullptr) 	return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, pClass, -1));
		}

		bool AddCallback(CallbackType type, CallbackHandler handler) const
		{
			using AddCallbackFn = bool (*)(IHook*, bool, CallbackHandler);
			static AddCallbackFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.AddCallback", reinterpret_cast<void**>(&func));
			return func(m_hook, static_cast<bool>(type), handler);
		}

		bool RemoveCallback(CallbackType type, CallbackHandler handler) const
		{
			using RemoveCallbackFn = bool (*)(IHook*, bool, CallbackHandler);
			static RemoveCallbackFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.RemoveCallback", reinterpret_cast<void**>(&func));
			return func(m_hook, static_cast<bool>(type), handler);
		}

		bool IsCallbackRegistered(bool type, CallbackHandler handler) const
		{
			using IsCallbackRegisteredFn = bool (*)(IHook*, bool, CallbackHandler);
			static IsCallbackRegisteredFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.IsCallbackRegistered", reinterpret_cast<void**>(&func));
			return func(m_hook, type, handler);
		}

		bool AreCallbacksRegistered() const
		{
			using AreCallbacksRegisteredFn = bool (*)(IHook*);
			static AreCallbacksRegisteredFn func = nullptr;
			if (func == nullptr) plugify::GetMethodPtr2("dynhook.AreCallbacksRegistered", reinterpret_cast<void**>(&func));
			return func(m_hook);
		}

	private:
		IHook* m_hook;
		void* m_func;
		void* m_class;
		int m_index;
	};

	template <class T>
	inline constexpr bool always_false_v = std::is_same_v<std::decay_t<T>, std::add_cv_t<std::decay_t<T>>>;

	template <typename T> requires (std::is_pointer_v<T>)
	inline T GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = T (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentPointer", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <typename T>
	inline T GetArgument(IHook& hook, size_t index)
	{
		static_assert(always_false_v<T>, "GetArgument specialization required");
	}

	template <>
	inline bool GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = bool (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentBool", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline int8_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = int8_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentInt8", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline uint8_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = uint8_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentUInt8", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline int16_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = int16_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentInt16", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline uint16_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = uint16_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentUInt16", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline int32_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = int32_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentInt32", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline uint32_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = uint32_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentUInt32", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline int64_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = int64_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentInt64", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline uint64_t GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = uint64_t (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentUInt64", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline float GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = float (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentFloat", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline double GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = double (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentDouble", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline const char* GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = const char* (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentString", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <>
	inline const wchar_t* GetArgument(IHook& hook, size_t index)
	{
		using GetArgumentFn = const wchar_t* (*)(IHook*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetArgumentWString", reinterpret_cast<void**>(&func));
		return func(&hook, index);
	}

	template <typename T> requires(std::is_pointer_v<T>)
	inline void SetArgument(IHook& hook, size_t index, T value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, T);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentPointer", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <typename T>
	inline void SetArgument(IHook& hook, size_t index, T value)
	{
		static_assert(always_false_v<T>, "SetArgument specialization required");
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, bool value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, bool);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentBool", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, int8_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, int8_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentInt8", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, uint8_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, uint8_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentUInt8", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, int16_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, int16_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentInt16", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook & hook, size_t index, uint16_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, uint16_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentUInt16", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, int32_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, int32_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentInt32", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, uint32_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, uint32_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentUInt32", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, int64_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, int64_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentInt64", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, uint64_t value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, uint64_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentUInt64", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, float value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, float);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentFloat", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, double value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, double);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentDouble", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, const char* value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, const char*);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentString", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <>
	inline void SetArgument(IHook& hook, size_t index, const wchar_t* value)
	{
		using SetArgumentFn = void (*)(IHook*, size_t, const wchar_t*);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetArgumentWString", reinterpret_cast<void**>(&func));
		func(&hook, index, value);
	}

	template <typename T> requires(std::is_pointer_v<T>)
	inline T GetReturn(IHook& hook)
	{
		using GetReturnFn = T (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnPointer", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <typename T>
	inline T GetReturn(IHook& hook)
	{
		static_assert(always_false_v<T>, "GetReturn specialization required");
	}

	template <>
	inline bool GetReturn(IHook& hook)
	{
		using GetReturnFn = bool (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnBool", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline int8_t GetReturn(IHook& hook)
	{
		using GetReturnFn = int8_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnInt8", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline uint8_t GetReturn(IHook& hook)
	{
		using GetReturnFn = uint8_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnUInt8", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline int16_t GetReturn(IHook& hook)
	{
		using GetReturnFn = int16_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnInt16", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline uint16_t GetReturn(IHook& hook)
	{
		using GetReturnFn = uint16_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnUInt16", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline int32_t GetReturn(IHook& hook)
	{
		using GetReturnFn = int32_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnInt32", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline uint32_t GetReturn(IHook& hook)
	{
		using GetReturnFn = uint32_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnUInt32", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline int64_t GetReturn(IHook& hook)
	{
		using GetReturnFn = int64_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnInt64", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline uint64_t GetReturn(IHook& hook)
	{
		using GetReturnFn = uint64_t (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnUInt64", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline float GetReturn(IHook& hook)
	{
		using GetReturnFn = float (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnFloat", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline double GetReturn(IHook& hook)
	{
		using GetReturnFn = double (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnDouble", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline const char* GetReturn(IHook& hook)
	{
		using GetReturnFn = const char* (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnString", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <>
	inline const wchar_t* GetReturn(IHook& hook)
	{
		using GetReturnFn = const wchar_t* (*)(IHook*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.GetReturnWString", reinterpret_cast<void**>(&func));
		return func(&hook);
	}

	template <typename T> requires(std::is_pointer_v<T>)
	inline void SetReturn(IHook& hook, T value)
	{
		using SetReturnFn = void (*)(IHook*, T);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnPointer", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <typename T>
	inline void SetReturn(IHook& hook, T value)
	{
		static_assert(always_false_v<T>, "SetReturn specialization required");
	}

	template <>
	inline void SetReturn(IHook& hook, bool value)
	{
		using SetReturnFn = void (*)(IHook*, bool);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnBool", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, int8_t value)
	{
		using SetReturnFn = void (*)(IHook*, int8_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnInt8", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, uint8_t value)
	{
		using SetReturnFn = void (*)(IHook*, uint8_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnUInt8", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, int16_t value)
	{
		using SetReturnFn = void (*)(IHook*, int16_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnInt16", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, uint16_t value)
	{
		using SetReturnFn = void (*)(IHook*, uint16_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnUInt16", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, int32_t value)
	{
		using SetReturnFn = void (*)(IHook*, int32_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnInt32", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, uint32_t value)
	{
		using SetReturnFn = void (*)(IHook*, uint32_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnUInt32", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, int64_t value)
	{
		using SetReturnFn = void (*)(IHook*, int64_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnInt64", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, uint64_t value)
	{
		using SetReturnFn = void (*)(IHook*, uint64_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnUInt64", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, float value)
	{
		using SetReturnFn = void (*)(IHook*, float);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnFloat", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, double value)
	{
		using SetReturnFn = void (*)(IHook*, double);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnDouble", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, const char* value)
	{
		using SetReturnFn = void (*)(IHook*, const char*);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnString", reinterpret_cast<void**>(&func));
		func(&hook, value);
	}

	template <>
	inline void SetReturn(IHook& hook, const wchar_t* value)
	{
		using SetReturnFn = void (*)(IHook*, const wchar_t*);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plugify::GetMethodPtr2("dynhook.SetReturnWString", reinterpret_cast<void**>(&func));
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