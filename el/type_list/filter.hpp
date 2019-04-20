#ifndef ELMETA_FILTER_HPP
	#define ELMETA_FILTER_HPP
	#include "el/types/type_c.hpp"
	#include "el/fwd/type_list/type_list.hpp"

	namespace el {
		namespace impl {
			template<typename Filter, typename ...Keep>
			constexpr auto filter(
				el::Type_c<el::type_list<Keep...>> const &result,
				el::Type_c<el::type_list<>> const &,
				Filter&&
			) noexcept
			{
				return result;
			}

			template<typename Filter, typename ...Keep, typename THead, typename ...TRest>
			constexpr auto filter(
				el::Type_c<el::type_list<Keep...>> const &,
				el::Type_c<el::type_list<THead, TRest...>> const &,
				Filter&& f
			) noexcept
			{
				return el::impl::filter<Filter>(
					el::type_c<el::conditional_t<
						decltype(f(el::type_c<THead>))::value,
						el::type_list<Keep..., THead>,
						el::type_list<Keep...>
					>>,
					el::type_c<el::type_list<TRest...>>,
					std::forward<Filter>(f)
				);
			}
		} // impl
	} // el
#endif // ELMETA_FILTER_HPP