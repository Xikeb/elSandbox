#ifndef ELECS_SIGNATURE_CONCEPT_HPP
	#define ELECS_SIGNATURE_CONCEPT_HPP
	#include "el/types/is_valid.hpp"

	namespace ecs {
		namespace impl {
			template<typename T, typename = el::void_t<>>
			struct hasSettings: el::true_c {
			};
		} // impl

		template<typename TSettings>
		struct SignatureConcept {
			using Settings = TSettings;
			constexpr SignatureConcept() = default;

			constexpr static auto canCompare = el::is_valid([](auto &&sig) -> bool {
				return sig.compare(SignatureBitset<Settings>{});
			});

			template<typename TSig>
			constexpr auto operator()(TSig &&sig) {
				return canCompare(sig);
			}
		};

		template<typename TSettings>
		constexpr static ecs::SignatureConcept<TSettings> isSigFunctor{};

		template<typename TSettings, typename TSig>
		constexpr auto isSignature(const TSig&& sig = TSig()) noexcept
		{
			return isSigFunctor<TSettings>(std::forward<TSig>(sig));
		}
	} // ecs
#endif // ELECS_SIGNATURE_CONCEPT_HPP