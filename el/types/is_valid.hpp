#ifndef ELMETA_IS_VALID_HPP
	#define ELMETA_IS_VALID_HPP
	#include "el/types/integral_c.hpp"
	
	namespace el {
		namespace impl {
			template<typename TF, typename ...Args, typename = decltype(
				std::declval<TF>()(std::declval<Args>()...)
			)>
			constexpr auto is_valid(Args&&..., int) noexcept {
				return el::true_c{};
			}

			template<typename TF, typename ...Args>
			constexpr auto is_valid(Args&&..., ...) noexcept {
				return el::false_c{};
			}

			template<typename TF>
			struct is_valid_functor {
				constexpr is_valid_functor() = default;
				template<typename ...Args>
				constexpr auto operator()(Args&&... args) const noexcept
				{
					return el::impl::is_valid<TF>(std::forward<Args>(args)..., int{});
				}
			};
		} // impl

		template<typename TF>
		constexpr auto is_valid(TF&& f) noexcept
		{
			return el::impl::is_valid_functor<TF>();
		}
	} // el
#endif // ELMETA_IS_VALID_HPP