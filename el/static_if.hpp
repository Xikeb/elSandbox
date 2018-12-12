#ifndef ELMETA_STATIC_IF_HPP
	#define ELMETA_STATIC_IF_HPP
	#include "el/types/integral_c.hpp"

	namespace el {
		namespace impl {
			template<typename Callback>
			struct StaticIfResult {
				Callback c;

				constexpr StaticIfResult(Callback &&f): c(std::forward<Callback>(f))
				{
				}

				template<typename F, typename ...Args>
				constexpr auto then(F&&, Args&&...) const noexcept
				{
					return *this;
				}

				template<typename F, typename ...Args>
				constexpr auto otherwise(F&&, Args&&...) const noexcept
				{
					return *this;
				}

				template<typename ...Args>
				constexpr auto operator()(Args&&... args) const noexcept
				{
					return c(std::forward<Args>(args)...);
				}
			};

			template<typename Callback>
			constexpr auto make_staticIfResult(Callback &&c) noexcept
			{
				return StaticIfResult<Callback>(std::forward<Callback>(c));
			}

			//From Romeo Vittorio's static_if
			template<bool Value>
			struct StaticIf;

			template<>
			struct StaticIf<false> {
				const bool value = false;

				constexpr operator bool() const noexcept
				{
					return false;
				}

				template<typename Callback>
				constexpr auto then(Callback&&) const noexcept
				{
					return *this;
				}

				template<typename Callback>
				constexpr auto otherwise(Callback &&c) const noexcept
				{
					return make_staticIfResult(std::forward<Callback>(c));
				}

				template<typename Lhs, typename Rhs>
				constexpr auto ternary(Lhs &&, Rhs &&rhs) const noexcept
				{
					return std::forward<Rhs>(rhs);
				}
			};

			template<>
			struct StaticIf<true> {
				const bool value = true;

				constexpr operator bool() const noexcept
				{
					return true;
				}

				template<typename Callback, typename ...Args>
				constexpr auto then(Callback &&c, Args&&... args) const noexcept
				{
					return make_staticIfResult(std::forward<Callback>(c));
				}

				template<typename Callback, typename ...Args>
				constexpr auto otherwise(Callback&&, Args&&...) const noexcept
				{
					return *this;
				}

				template<typename Lhs, typename Rhs>
				constexpr auto ternary(Lhs &&lhs, Rhs &&) const noexcept
				{
					return std::forward<Lhs>(lhs);
				}
			};
		} // impl

		template<bool Value>
		constexpr auto static_if(el::bool_c<Value>) noexcept
		{
			return el::impl::StaticIf<Value>();
		}

		template<bool Value>
		constexpr auto static_if() noexcept
		{
			return el::impl::StaticIf<Value>();
		}
		// constexpr auto static_if(bool value) noexcept
		// {
		// 	return el::impl::StaticIf<value>();
		// }
	} // el
#endif // ELMETA_STATIC_IF_HPP