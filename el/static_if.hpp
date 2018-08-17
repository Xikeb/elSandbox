#ifndef ELMETA_STATIC_IF_HPP
	#define ELMETA_STATIC_IF_HPP

	namespace el {
		namespace impl {
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

				//Return *this, in order to be able to 
				// attach the <Otherwise> part to the expression
				template<typename Callback, typename ...Args>
				constexpr auto then(Callback&&, Args&&...) const noexcept
				{
					return *this;
				}

				template<typename Callback, typename ...Args>
				constexpr auto otherwise(Callback &&c, Args&&... args) const noexcept
				{
					c(std::forward<Args>(args)...);
					return *this;
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
					c(std::forward<Args>(args)...);
					return *this;
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