#pragma once
#include "el/conditional.hpp"
#include "el/types/is_valid.hpp"
#include "el/types/void.hpp"

namespace ecs {
	namespace impl {
		template<typename T, typename = el::void_t<typename T::Settings>>
		struct hasSettings: el::true_c {
		};

		template<typename T>
		struct hasSettings<T>: el::false_c {
		};
	} // impl

	template<typename TSettings>
	struct SignatureConcept {
		using Settings = TSettings;
		constexpr SignatureConcept() = default;

		constexpr static auto canCompare = el::is_valid([](auto &&sig) -> bool {
			return sig.compare(SignatureBitset<Settings>{});
		});

		constexpr static auto canGetBitset = el::is_valid(
			[](auto &&sig) -> ecs::SignatureBitset<Settings> {
				return sig.getBitset();
			}
		);

		template<typename TSig>
		constexpr auto operator()(TSig &&sig) const noexcept {
			return canCompare(sig);
		}
	};

	template<typename TSettings>
	constexpr static ecs::SignatureConcept<TSettings> isSigFunctor{};

	template<
		typename TSig,
		typename TSettings = el::conditional_t<
			ecs::impl::hasSettings<TSig>::value,
			typename TSig::Settings,
			void
		>
	>
	constexpr auto isSignature(TSig&& sig = TSig()) noexcept
	{
		return isSigFunctor<TSettings>(std::forward<TSig>(sig));
	}

	template<
		typename TSig,
		typename TSettings = el::conditional_t<
			ecs::impl::hasSettings<TSig>::value,
			typename TSig::Settings,
			void
		>
	>
	constexpr auto canGetBitset(TSig&& sig = TSig()) noexcept
	{
		return isSigFunctor<TSettings>.canGetBitset(std::forward<TSig>(sig));
	}
} // ecs