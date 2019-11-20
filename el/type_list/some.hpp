#pragma once
#include <utility>

namespace el {
	namespace impl {
		template<typename TList, typename TF, typename ...Args>
		constexpr bool some(el::size_c<(TList::size - 1)> i, TF&& f, Args&&... args, int = 0)
		{
			return f(
				el::type_c<typename TList::template At<(TList::size - 1)>>,
				i, std::forward<Args>(args)...
			);
		}

		template<typename TList, std::size_t Idx, typename TF, typename ...Args>
		constexpr bool some(el::size_c<Idx> i, TF&& f, Args&&... args, ...)
		{
			if (f(
				el::type_c<typename TList::template At<Idx>>,
				i, std::forward<Args>(args)...
			))
				return true;
			return some<TList>(
				el::size_c<Idx + 1>(),
				std::forward<TF>(f),
				std::forward<Args>(args)...,
				int{}
			);
		}
	} // impl
} // el