#ifndef ELECS_SIGNATURE_HPP
	#define ELECS_SIGNATURE_HPP
	#include <cstdlib>
	#include <bitset>
	#include <tuple>
	#include "el/detail/and.hpp"
	#include "el/type_list/type_list.hpp"
	#include "el/types/is_valid.hpp"
	
	namespace ecs {
		template<std::size_t Length>
		struct SignatureConcept {
			using Bitset = std::bitset<Length>;

			constexpr SignatureConcept() = default;

			constexpr static auto canCompare = el::is_valid([](auto &&sig) -> bool {
				return sig.compare(Bitset{});
			});

			template<typename TSig>
			constexpr auto operator()(TSig&& sig) {
				return canCompare(sig);
			}
		};

		template<std::size_t Length>
		constexpr static ecs::SignatureConcept<Length> isSigFunctor{};

		template<typename TSig>
		constexpr auto isSignature(TSig &&sig) noexcept -> decltype(isSigFunctor<TSig>) {
			return isSigFunctor<TSig::length()>(std::forward<TSig>(sig));
		}

		template<typename TList>
		class Signature {
		public:
			using List = TList;
			constexpr Signature() = default;

			constexpr static std::size_t length() noexcept {
				return List::size;
			}

			using Bitset = std::bitset<length()>;

			bool compare(Bitset const &othPrint) const
			{
				return (this->_print & othPrint) == this->_print;
			}
		private:
			Bitset _print;
		};

		template<typename ...TSigs>
		class SignatureAnd {
		public:
			static_assert(el::detail::_and<
					isSignature(TSigs())...
				>::value,
				"Template arguments must all"
				" implement the concept of Signature.");
			using Signatures = el::type_list<TSigs...>;
			constexpr SignatureAnd() = default;

			constexpr static std::size_t length() noexcept {
				return Signatures::Current::List::size;
			}
			using Bitset = std::bitset<length()>;

			bool compare(Bitset const &othPrint) const
			{
				return this->compareImpl<0>(othPrint);
			}

		private:
			std::tuple<TSigs...> _sigs;

			template<std::size_t I>
			bool compareImpl(Bitset const &othPrint) const noexcept {
				if (I >= Signatures::size)
					return true;
				else if (std::get<I>(this->_sigs).compare(
					othPrint
				))
					return this->compareImpl<I + 1>(othPrint);
				return false;
			}
		};

		template<typename ...TSigs>
		class SignatureOr {
		public:
			// static_assert(el::detail::_and<

			// 	>::value,
			// 	"Template arguments must all"
			// 	" implement the concept of Signature.");
			using Signatures = el::type_list<TSigs...>;
			constexpr SignatureOr() = default;

			constexpr static std::size_t length() noexcept {
				return Signatures::Current::List::size;
			}
			using Bitset = std::bitset<length()>;

			bool compare(Bitset const &othPrint) const
			{
				return this->compareImpl<0>(othPrint);
			}

		private:
			std::tuple<TSigs...> _sigs;

			template<std::size_t I>
			bool compareImpl(Bitset const &othPrint) const noexcept {
				if (I >= Signatures::size)
					return false;
				else if (std::get<I>(this->_sigs).compare(
					othPrint
				))
					return true;
				return this->compareImpl<I + 1>(othPrint);
			}
		};

		template<typename TSig>
		class SignatureNot {
		public:
			// static_assert(el::detail::_and<

			// 	>::value,
			// 	"Template arguments must all"
			// 	" implement the concept of Signature.");
			constexpr SignatureNot() = default;

			using Signature = TSig;
			using Signature::length;
			// constexpr static std::size_t length() noexcept {
			// 	return Signatures::Current::List::size;
			// }
			using Bitset = std::bitset<length()>;

			bool compare(Bitset const &othPrint) const
			{
				return !this->_sig.compare(othPrint);
			}

		private:
			Signature _sig;
		};

		template<typename TSigCond, typename TSigTrue, typename TSigFalse>
		class SignatureConditional {
		public:
			// static_assert(el::detail::_and<

			// 	>::value,
			// 	"Template arguments must all"
			// 	" implement the concept of Signature.");
			// using Signatures = el::type_list<TSigs...>;
			using Condition = TSigCond;
			using IfSignature = TSigTrue;
			using ElseSignature = TSigFalse;
			constexpr SignatureConditional() = default;

			using Condition::length;
			using Bitset = std::bitset<length()>;

			bool compare(Bitset const &othPrint) const
			{
				return this->_cond.compare(othPrint)
				? this->_if.compare(othPrint)
				: this->_else.compare(othPrint);
			}

		private:
			Condition _cond;
			IfSignature _if;
			ElseSignature _else;
		};
	} // ecs
#endif // ELECS_SIGNATURE_HPP