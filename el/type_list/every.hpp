#pragma once
#include <utility>

namespace el {
	namespace impl {
		template<typename TList, typename TF, typename ...Args>
		constexpr bool every(el::size_c<(TList::size - 1)> i, TF&& f, Args&&... args)
		{
			return f(
				el::type_c<typename TList::template At<(TList::size - 1)>>,
				i, std::forward<Args>(args)...
			);
		}

		template<typename TList, std::size_t Idx, typename TF, typename ...Args>
		constexpr bool every(el::size_c<Idx> i, TF&& f, Args&&... args)
		{
			if (!f(
				el::type_c<typename TList::template At<Idx>>,
				i, std::forward<Args>(args)...
			))
				return false;
			return every<TList>(
				el::size_c<Idx + 1>(),
				std::forward<TF>(f),
				std::forward<Args>(args)...
			);
		}
	} // impl
} // el