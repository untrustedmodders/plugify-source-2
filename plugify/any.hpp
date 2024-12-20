#pragma once

#include <plugify/numerics.hpp>
#include <plugify/string.hpp>
#include <plugify/variant.hpp>
#include <plugify/vector.hpp>

namespace plg {
	struct invalid {};
	struct none {};
	union function { void* ptr; };

	using any = variant<
			invalid,

			none,
			bool,
			char,
			char16_t,
			int8_t,
			int16_t,
			int32_t,
			int64_t,
			uint8_t,
			uint16_t,
			uint32_t,
			uint64_t,
			void*,
			float,
			double,
			function,
			string,
			variant<none>,
			vector<bool>,
			vector<char>,
			vector<char16_t>,
			vector<int8_t>,
			vector<int16_t>,
			vector<int32_t>,
			vector<int64_t>,
			vector<uint8_t>,
			vector<uint16_t>,
			vector<uint32_t>,
			vector<uint64_t>,
			vector<void*>,
			vector<float>,
			vector<double>,
			vector<string>,
			vector<variant<none>>,
			vec2,
			vec3,
			vec4
			//mat4x4
			>;


	template<typename T, typename = void>
	struct has_arithmetic_value_type : std::false_type {};

	template<typename T>
	struct has_arithmetic_value_type<T, std::void_t<typename T::value_type>>
		: std::is_arithmetic<typename T::value_type> {};

	template<typename T>
	inline constexpr bool has_arithmetic_value_type_v = has_arithmetic_value_type<T>::value;

	template<typename T, typename = void>
	struct is_vector : std::false_type {};

	template<typename T>
	struct is_vector<T, std::void_t<typename T::value_type, typename T::allocator_type>>
		: std::is_same<plg::vector<typename T::value_type, typename T::allocator_type>, T> {};

	template<typename T>
	inline constexpr bool is_vector_v = is_vector<T>::value;


} // namespace plg
