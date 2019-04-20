#pragma once
#include <utility>

namespace el {
	namespace impl {
		template<typename TList, typename Acc, typename TF, typename ...Args>
		constexpr auto reduce(el::size_c<0> i, TF&& f, Acc&& acc, Args&&... args, int = 0)
		{
			return f(
				// el::type_c<typename TList::template At<(TList::size - 1)>>,
				el::type_c<typename TList::template At<0>>,
				i,
				std::forward<Acc>(acc),
				std::forward<Args>(args)...
			);
		}

		template<typename TList, typename Acc, std::size_t Idx, typename TF, typename ...Args>
		constexpr auto reduce(el::size_c<Idx> i, TF&& f, Acc&& acc, Args&&... args, ...)
		{
			return f(
				el::type_c<typename TList::template At<Idx>>,
				i,
				reduce<TList>(
					el::size_c<Idx - 1>{},
					std::forward<TF>(f),
					std::forward<Acc>(acc),
					std::forward<Args>(args)...,
					int{}
				),
				std::forward<Args>(args)...
			);
		}
	} // impl
} // el