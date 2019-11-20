#ifndef ELMETA_FOR_EACH_HPP
	#define ELMETA_FOR_EACH_HPP
	#include <utility>
	#include "el/types/void.hpp"

	namespace el {
		namespace impl {
			template<typename TList,
						// size_t Idx,
						typename TF,
						typename ...Args
			> constexpr auto for_each(el::size_c<(TList::size - 1)>, TF&& f, Args&&... args) noexcept
			{
				std::forward<TF>(f)(
					el::type_c<typename TList::template At<(TList::size - 1)>>,
					el::size_c<(TList::size - 1)>{},
					std::forward<Args>(args)...
				);
				return std::move(std::forward<TF>(f));
			}

			template<typename TList,
						size_t Idx,
						typename TF,
						typename ...Args,
						typename = el::enable_if_t<(Idx + 1 < TList::size), void>
			> constexpr auto for_each(el::size_c<Idx>, TF&& f, Args&&... args) noexcept
			{
				std::forward<TF>(f)(
					el::type_c<typename TList::template At<Idx>>,
					el::size_c<Idx>{},
					std::forward<Args>(args)...
				);
				return el::impl::for_each<TList>(
					el::size_c<Idx + 1>{},
					std::forward<TF>(f),
					std::forward<Args>(args)...
				);
			}
		} // impl
	} // el
#endif // ELMETA_FOR_EACH_HPP