#pragma once

#include <type_traits>
#include <tuple>


namespace IoCpp
{
	namespace tmp
	{

		template <typename TA, typename TB>
		constexpr int count_same()
		{
			if constexpr (std::is_same<TA, TB>::value)
				return 1;
			else
				return 0;
		}

		template <typename TT, typename... TArg>
		constexpr int count_same_in_pack()
		{
			return (count_same<TT, TArg>() + ... + 0);
		}

		template <size_t N, typename... TArg>
		constexpr int count_same_in_pack()
		{
			using tuple_t = std::tuple<TArg...>;
			return count_same_in_pack<std::tuple_element_t<N, tuple_t>, TArg...>();
		}

		template <typename... TArg, size_t... N>
		constexpr int count_all_in_pack(std::index_sequence<N...>)
		{
			return (count_same_in_pack<N, TArg...>() + ... + 0);
		}

		template <int C>
		constexpr char alert()
		{
			return static_cast<char>(C + 256);
		}

		template <typename... TArg>
		constexpr bool check_no_duplicate()
		{
			constexpr auto nCount = count_all_in_pack<TArg...>(std::index_sequence_for<TArg...>());
			//alert<nCount>();
			return nCount == sizeof...(TArg);
		}



	}
}