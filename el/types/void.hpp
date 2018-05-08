#ifndef ELMETA_VOID_HPP
	#define ELMETA_VOID_HPP
	namespace el {
		namespace impl {
			template<typename ...Ts>
			struct void_t {
				using Type = void;
			};
		} // impl
		template<typename ...Ts>
		using void_t = typename el::impl::void_t<Ts...>::Type;
	} // el
#endif // ELMETA_VOID_HPP