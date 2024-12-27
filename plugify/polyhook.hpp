// PolyHook_2_0
// Copyright (C) 2023-2024 PolyHook_2_0
// https://github.com/stevemk14ebr/PolyHook_2_0

#pragma once

#include "cpp_plugin.hpp"

namespace poly
{
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
		String
	};

	enum class ReturnAction : int
	{
		Ignored,  // handler didn't take any action
		Handled,  // we did something, but real function should still be called
		Override, // call real function, but use my return value
		Supercede // skip real function; use my return value
	};

	enum class CallbackType : bool
	{
		Pre,  // callback will be executed before the original function
		Post  // callback will be executed after the original function
	};

	class IHook;
	class Params;
	class Return;

	using CallbackHandler = ReturnAction(CallbackType, Params&, int, Return&);

	class CHook
	{
	private:
		CHook(IHook* pHook, void* pFunc, void* pClass, int index) : m_hook(pHook), m_func(pFunc), m_class(pClass), m_index(index)
		{}

	public:
		~CHook()
		{
			// detour
			if (!m_class)
			{
				using UnhookDetourFn = bool (*)(void*);
				static UnhookDetourFn func = nullptr;
				if (func == nullptr) plg::GetMethodPtr2("polyhook.UnhookDetour", reinterpret_cast<void**>(&func));
				func(m_func);
			}
			else if (m_func)
			{
				using UnhookVirtualByFuncFn = bool (*)(void*, void*);
				static UnhookVirtualByFuncFn func = nullptr;
				if (func == nullptr) plg::GetMethodPtr2("polyhook.UnhookVirtualByFunc", reinterpret_cast<void**>(&func));
				func(m_class, m_func);
			}
			else
			{
				using UnhookVirtualFn = bool (*)(void*, int);
				static UnhookVirtualFn func = nullptr;
				if (func == nullptr) plg::GetMethodPtr2("polyhook.UnhookVirtual", reinterpret_cast<void**>(&func));
				func(m_class, m_index);
			}
		}

		static std::unique_ptr<CHook> CreateDetourHook(void* pFunc, DataType returnType, const plg::vector<DataType>& arguments)
		{
			using HookDetourFn = IHook* (*)(void*, DataType, const plg::vector<DataType>&);
			static HookDetourFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.HookDetour", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pFunc, returnType, arguments);
			if (pHook == nullptr) 	return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, nullptr, -1));
		}

		static std::unique_ptr<CHook> CreateVirtualHook(void* pClass, int index, DataType returnType, const plg::vector<DataType>& arguments)
		{
			using HookVirtualFn = IHook* (*)(void*, int, DataType, const plg::vector<DataType>&);
			static HookVirtualFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.HookVirtual", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pClass, index, returnType, arguments);
			if (pHook == nullptr) return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, nullptr, pClass, index));
		}

		static std::unique_ptr<CHook> CreateHookVirtualByFunc(void* pClass, void* pFunc, DataType returnType, const plg::vector<DataType>& arguments)
		{
			using HookVirtualByFuncFn = IHook* (*)(void*, void*, DataType, const plg::vector<DataType>&);
			static HookVirtualByFuncFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.HookVirtualByFunc", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pClass, pFunc, returnType, arguments);
			if (pHook == nullptr) return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, pClass, -1));
		}

		static std::unique_ptr<CHook> FindDetour(void* pFunc)
		{
			using FindDetourFn = IHook* (*)(void*);
			static FindDetourFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.FindDetour", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pFunc);
			if (pHook == nullptr) return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, nullptr, -1));
		}

		static std::unique_ptr<CHook> FindVirtual(void* pClass, int index)
		{
			using FindVirtualFn = IHook* (*)(void*, int);
			static FindVirtualFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.FindVirtual", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pClass, index);
			if (pHook == nullptr) return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, nullptr, pClass, index));
		}

		static std::unique_ptr<CHook> FindVirtualByFunc(void* pClass, void* pFunc)
		{
			using FindVirtualByFuncFn = IHook* (*)(void*, void*);
			static FindVirtualByFuncFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.FindVirtualByFunc", reinterpret_cast<void**>(&func));
			IHook* pHook = func(pClass, pFunc);
			if (pHook == nullptr) return nullptr;
			return std::unique_ptr<CHook>(new CHook(pHook, pFunc, pClass, -1));
		}

		bool AddCallback(CallbackType type, CallbackHandler handler) const
		{
			using AddCallbackFn = bool (*)(IHook*, CallbackType, CallbackHandler);
			static AddCallbackFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.AddCallback", reinterpret_cast<void**>(&func));
			return func(m_hook, type, handler);
		}

		bool RemoveCallback(CallbackType type, CallbackHandler handler) const
		{
			using RemoveCallbackFn = bool (*)(IHook*, CallbackType, CallbackHandler);
			static RemoveCallbackFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.RemoveCallback", reinterpret_cast<void**>(&func));
			return func(m_hook, type, handler);
		}

		bool IsCallbackRegistered(CallbackType type, CallbackHandler handler) const
		{
			using IsCallbackRegisteredFn = bool (*)(IHook*, CallbackType, CallbackHandler);
			static IsCallbackRegisteredFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.IsCallbackRegistered", reinterpret_cast<void**>(&func));
			return func(m_hook, type, handler);
		}

		bool AreCallbacksRegistered() const
		{
			using AreCallbacksRegisteredFn = bool (*)(IHook*);
			static AreCallbacksRegisteredFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.AreCallbacksRegistered", reinterpret_cast<void**>(&func));
			return func(m_hook);
		}

		void* GetAddress() const
		{
			using GetCallbackAddrFn = void* (*)(IHook*);
			static GetCallbackAddrFn func = nullptr;
			if (func == nullptr) plg::GetMethodPtr2("polyhook.GetCallbackAddr", reinterpret_cast<void**>(&func));
			return func(m_hook);
		}

	private:
		IHook* m_hook;
		void* m_func;
		void* m_class;
		int m_index;
	};

	inline void* FindOriginalAddr(void* pClass, void* pAddr)
	{
		using FindOriginalAddrFn = IHook* (*)(void*, void*);
		static FindOriginalAddrFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.FindOriginalAddr", reinterpret_cast<void**>(&func));
		return func(pClass, pAddr);
	}

	inline int GetVTableIndex(void* pFunc)
	{
		using GetVTableIndexFn = int (*)(void*);
		static GetVTableIndexFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetVTableIndex", reinterpret_cast<void**>(&func));
		return func(pFunc);
	}

	template <class T>
	inline constexpr bool always_false_v = std::is_same_v<std::decay_t<T>, std::add_cv_t<std::decay_t<T>>>;

	template <typename T> requires (std::is_pointer_v<T>)
	inline T GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = T (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentPointer", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <typename T>
	inline T GetArgument(Params& params, size_t index)
	{
		static_assert(always_false_v<T>, "GetArgument specialization required");
	}

	template <>
	inline bool GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = bool (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentBool", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline int8_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = int8_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentInt8", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline uint8_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = uint8_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentUInt8", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline int16_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = int16_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentInt16", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline uint16_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = uint16_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentUInt16", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline int32_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = int32_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentInt32", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline uint32_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = uint32_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentUInt32", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline int64_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = int64_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentInt64", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline uint64_t GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = uint64_t (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentUInt64", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline float GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = float (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentFloat", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline double GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = double (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentDouble", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline const char* GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = const char* (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentString", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <>
	inline const wchar_t* GetArgument(Params& params, size_t index)
	{
		using GetArgumentFn = const wchar_t* (*)(Params*, size_t);
		static GetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetArgumentWString", reinterpret_cast<void**>(&func));
		return func(&params, index);
	}

	template <typename T> requires(std::is_pointer_v<T>)
	inline void SetArgument(Params& params, size_t index, T value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, T);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentPointer", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <typename T>
	inline void SetArgument(Params& params, size_t index, T value)
	{
		static_assert(always_false_v<T>, "SetArgument specialization required");
	}

	template <>
	inline void SetArgument(Params& params, size_t index, bool value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, bool);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentBool", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, int8_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, int8_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentInt8", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, uint8_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, uint8_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentUInt8", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, int16_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, int16_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentInt16", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, uint16_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, uint16_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentUInt16", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, int32_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, int32_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentInt32", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, uint32_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, uint32_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentUInt32", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, int64_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, int64_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentInt64", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, uint64_t value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, uint64_t);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentUInt64", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, float value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, float);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentFloat", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, double value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, double);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentDouble", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, const char* value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, const char*);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentString", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <>
	inline void SetArgument(Params& params, size_t index, const wchar_t* value)
	{
		using SetArgumentFn = void (*)(Params*, size_t, const wchar_t*);
		static SetArgumentFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetArgumentWString", reinterpret_cast<void**>(&func));
		func(&params, index, value);
	}

	template <typename T> requires(std::is_pointer_v<T>)
	inline T GetReturn(Return& ret)
	{
		using GetReturnFn = T (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnPointer", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <typename T>
	inline T GetReturn(Return& ret)
	{
		static_assert(always_false_v<T>, "GetReturn specialization required");
	}

	template <>
	inline bool GetReturn(Return& ret)
	{
		using GetReturnFn = bool (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnBool", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline int8_t GetReturn(Return& ret)
	{
		using GetReturnFn = int8_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnInt8", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline uint8_t GetReturn(Return& ret)
	{
		using GetReturnFn = uint8_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnUInt8", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline int16_t GetReturn(Return& ret)
	{
		using GetReturnFn = int16_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnInt16", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline uint16_t GetReturn(Return& ret)
	{
		using GetReturnFn = uint16_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnUInt16", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline int32_t GetReturn(Return& ret)
	{
		using GetReturnFn = int32_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnInt32", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline uint32_t GetReturn(Return& ret)
	{
		using GetReturnFn = uint32_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnUInt32", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline int64_t GetReturn(Return& ret)
	{
		using GetReturnFn = int64_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnInt64", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline uint64_t GetReturn(Return& ret)
	{
		using GetReturnFn = uint64_t (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnUInt64", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline float GetReturn(Return& ret)
	{
		using GetReturnFn = float (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnFloat", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline double GetReturn(Return& ret)
	{
		using GetReturnFn = double (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnDouble", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline const char* GetReturn(Return& ret)
	{
		using GetReturnFn = const char* (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnString", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <>
	inline const wchar_t* GetReturn(Return& ret)
	{
		using GetReturnFn = const wchar_t* (*)(Return*);
		static GetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.GetReturnWString", reinterpret_cast<void**>(&func));
		return func(&ret);
	}

	template <typename T> requires(std::is_pointer_v<T>)
	inline void SetReturn(Return& ret, T value)
	{
		using SetReturnFn = void (*)(Return*, T);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnPointer", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <typename T>
	inline void SetReturn(Return& ret, T value)
	{
		static_assert(always_false_v<T>, "SetReturn specialization required");
	}

	template <>
	inline void SetReturn(Return& ret, bool value)
	{
		using SetReturnFn = void (*)(Return*, bool);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnBool", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, int8_t value)
	{
		using SetReturnFn = void (*)(Return*, int8_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnInt8", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, uint8_t value)
	{
		using SetReturnFn = void (*)(Return*, uint8_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnUInt8", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, int16_t value)
	{
		using SetReturnFn = void (*)(Return*, int16_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnInt16", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, uint16_t value)
	{
		using SetReturnFn = void (*)(Return*, uint16_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnUInt16", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, int32_t value)
	{
		using SetReturnFn = void (*)(Return*, int32_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnInt32", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, uint32_t value)
	{
		using SetReturnFn = void (*)(Return*, uint32_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnUInt32", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, int64_t value)
	{
		using SetReturnFn = void (*)(Return*, int64_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnInt64", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, uint64_t value)
	{
		using SetReturnFn = void (*)(Return*, uint64_t);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnUInt64", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, float value)
	{
		using SetReturnFn = void (*)(Return*, float);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnFloat", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, double value)
	{
		using SetReturnFn = void (*)(Return*, double);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnDouble", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, const char* value)
	{
		using SetReturnFn = void (*)(Return*, const char*);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnString", reinterpret_cast<void**>(&func));
		func(&ret, value);
	}

	template <>
	inline void SetReturn(Return& ret, const wchar_t* value)
	{
		using SetReturnFn = void (*)(Return*, const wchar_t*);
		static SetReturnFn func = nullptr;
		if (func == nullptr) plg::GetMethodPtr2("polyhook.SetReturnWString", reinterpret_cast<void**>(&func));
		func(&ret, value);
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

			static std::array<DataType, arity> args()
			{
				return {GetType<Args>()...};
			}

			static DataType ret()
			{
				return GetType<Ret>();
			}
		};

		template <typename Class, typename Ret, typename... Args>
		struct function_traits<Ret (Class::*)(Args...)>
		{
			static constexpr size_t arity = sizeof...(Args);

			static std::array<DataType, arity + 1> args()
			{
				std::array<DataType, arity> originalArgs = {GetType<Args>()...};
				std::array<DataType, arity + 1> result;
				result[0] = DataType::Pointer;
				std::copy(originalArgs.begin(), originalArgs.end(), result.begin() + 1);
				return result;
			}

			static DataType ret()
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

			static std::array<DataType, arity> args()
			{
				return {GetType<Args>()...};
			}

			static DataType ret()
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