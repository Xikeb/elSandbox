#pragma once

namespace el {
	namespace impl {
		template<typename ...Ts>
		struct void_t {
			using type = void;
		};
	} // impl
	template<typename ...Ts>
	using void_t = typename el::impl::void_t<Ts...>::type;
} // el