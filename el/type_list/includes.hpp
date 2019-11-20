/*
 * type_list.h
 *
 *  Created on: 22 nov. 2017
 *      Author: eliord
 */

#ifndef TYPE_LIST_INCLUDES_H_
# define TYPE_LIST_INCLUDES_H_

# include <type_traits>
# include "el/type_list/type_list.hpp"
# include "el/types/nothing.hpp"
# include "el/types/integral_c.hpp"
# include "el/types/void.hpp"
# include "el/conditional.hpp"
# include "el/is_same.hpp"

namespace el {
	namespace impl {
		template<bool NotEmpty, typename List, typename TElem>
		struct IncludesHelper;
		template<typename List, typename TElem>
		struct IncludesBuilder;

		template<bool Empty, typename List, typename TElem>
		struct IncludesHelper {
			using Result = el::false_c;
		};

		template<typename List, typename TElem>
		struct IncludesHelper<false, List, TElem> {
			using Result = typename el::conditional<
				el::is_same<typename List::Current, TElem>::value,
				el::true_c,
				typename el::impl::IncludesBuilder<
					typename List::Next,
					TElem
				>::Result
			>::type;
		};

		template<typename List, typename TElem>
		struct IncludesBuilder {
			using Result = typename el::impl::IncludesHelper<
				el::IsEnd<List>::value,
				List,
				TElem
			>::Result;
		};
	} // impl

	template<typename List, typename TElem>
	using Includes = typename el::impl::IncludesBuilder<List, TElem>::Result;
}
#endif /*TYPE_LIST_INCLUDES_H_*/
