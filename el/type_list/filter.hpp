#ifndef ELMETA_FILTER_HPP
	#define ELMETA_FILTER_HPP
	#include "el/types/type_c.hpp"
	#include "el/fwd/type_list/type_list.hpp"

	namespace el {
		namespace impl {
			template<typename Pred, typename ...Keep>
			constexpr auto filter(
				el::Type_c<el::type_list<Keep...>> const &,
				el::Type_c<el::type_list<>> const &,
				Pred&&
			) noexcept
			{
				return el::type_list<Keep...>{};
			}

			template<typename Pred, typename ...Keep, typename THead, typename ...TRest>
			constexpr auto filter(
				el::Type_c<el::type_list<Keep...>> const &,
				el::Type_c<el::type_list<THead, TRest...>> const &,
				Pred&& f
			) noexcept
			{
				return el::impl::filter<Pred>(
					el::type_c<el::conditional_t<
						decltype(f(el::type_c<THead>))::value,
						el::type_list<Keep..., THead>,
						el::type_list<Keep...>
					>>,
					el::type_c<el::type_list<TRest...>>,
					std::forward<Pred>(f)
				);
			}
		} // impl
	} // el
#endif // ELMETA_FILTER_HPP