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

				template<typename F>
				constexpr auto then(F&&) const noexcept
				{
					return *this;
				}

				template<typename F>
				constexpr auto otherwise(F&&) const noexcept
				{
					return *this;
				}

				template<bool NewValue>
				constexpr auto elif() const noexcept
				{
					return *this;
				}

				template<bool NewValue>
				constexpr auto elif(el::bool_c<NewValue>) const noexcept
				{
					return *this;
				}

				template<typename ...Args>
				constexpr auto operator()(Args&&... args) const noexcept
				{
					return c(std::forward<Args>(args)...);
				}
			};

			//From Romeo Vittorio's static_if
			template<bool Value>
			struct StaticIf;

			template<bool Value>
			StaticIf(el::bool_c<Value>) -> StaticIf<Value>;

			template<>
			struct StaticIf<false> {
				constexpr static bool value = false;

				constexpr StaticIf() = default;
				constexpr StaticIf(el::bool_c<value>) {}

				constexpr operator bool() const noexcept
				{
					return value;
				}

				template<typename Callback>
				constexpr auto then(Callback&&) const noexcept
				{
					return *this;
				}

				template<bool NewValue>
				constexpr auto elif() const noexcept
				{
					return StaticIf{el::bool_c<NewValue>{}};
				}

				template<bool NewValue>
				constexpr auto elif(el::bool_c<NewValue>) const noexcept
				{
					return StaticIf{el::bool_c<NewValue>{}};
				}

				template<typename Callback>
				constexpr auto otherwise(Callback &&c) const noexcept
				{
					return StaticIfResult{std::forward<Callback>(c)};
				}

				template<typename Lhs, typename Rhs>
				constexpr auto ternary(Lhs &&, Rhs &&rhs) const noexcept
				{
					return std::forward<Rhs>(rhs);
				}

				template<typename ...Args>
				constexpr void operator()(Args&&...) const noexcept
				{
					//Nothing happens cuz no <otherwise> was used
				}
			};

			template<>
			struct StaticIf<true> {
				constexpr static bool value = true;

				constexpr StaticIf() = default;
				constexpr StaticIf(el::bool_c<value>) {}

				constexpr operator bool() const noexcept
				{
					return value;
				}

				template<typename Callback>
				constexpr auto then(Callback &&c) const noexcept
				{
					return StaticIfResult{std::forward<Callback>(c)};
				}

				template<bool NewValue>
				constexpr auto elif() const noexcept
				{
					return *this;
				}

				template<bool NewValue>
				constexpr auto elif(el::bool_c<NewValue>) const noexcept
				{
					return *this;
				}

				template<typename Callback>
				constexpr auto otherwise(Callback&&) const noexcept
				{
					return *this;
				}

				template<typename Lhs, typename Rhs>
				constexpr auto ternary(Lhs &&lhs, Rhs &&) const noexcept
				{
					return std::forward<Lhs>(lhs);
				}

				template<typename ...Args>
				constexpr void operator()(Args&&...) const noexcept
				{
					//Nothing happens cuz no <then> was used
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
	} // el
#endif // ELMETA_STATIC_IF_HPP