#ifndef ELECS_SIGNATURE_HPP
	#define ELECS_SIGNATURE_HPP
	#include <cstdlib>
	#include <bitset>
	#include <tuple>
	#include <iostream>
	#include "el/detail/and.hpp"
	#include "Settings.hpp"
	#include "SignatureBitset.hpp"
	#include "SignatureConcept.hpp"
	#include "el/type_list/type_list.hpp"
	#include "el/types/type_c.hpp"
	
	namespace ecs {
		template<typename TSettings, typename ...TTypes>
		class Signature {
		public:
			using Settings = TSettings;
			using Types = el::type_list<TTypes...>;
			using Components = el::type_of<decltype(Types::filter(Settings::ComponentList::has))>;
			using Tags = el::type_of<decltype(Types::filter(Settings::TagList::has))>;
			using Required = Types;

			constexpr Signature()
			{
				Components::for_each([&](auto &e, auto &) {
					this->_sto.template enableComponent<typename decltype(+e)::type>();
				});
				Tags::for_each([&](auto &e, auto &) {
					this->_sto.template enableTag<typename decltype(+e)::type>();
				});
			}

			constexpr static std::size_t componentCount = Components::size;
			constexpr static std::size_t tagCount = Tags::size;

			bool compare(SignatureBitset<Settings> const &othPrint) const noexcept
			{
				return this->_sto.matches(othPrint);
			}
		private:
			SignatureBitset<Settings> _sto;
		};

		class SignatureTrue {
		public:
			template<typename TSettings>
			bool compare(SignatureBitset<TSettings> const &) const noexcept
			{
				return true;
			}
		};

		template<typename ...TSigs>
		class SignatureAnd {
		public:
			static_assert(el::detail::andf(isSignature(TSigs())...),
				"Operands of signature logic classes must all "
				"implement the concept of Signature.");

			using SignatureList = el::type_list<TSigs...>;
			using Settings = typename SignatureList::First::Settings;
			constexpr SignatureAnd()
			{
			}

			constexpr static std::size_t length = SignatureList::Current::List::size;

			using Bitset = std::bitset<length>;

			bool compare(Bitset const &othPrint) const
			{
				return this->compareImpl<0>(othPrint);
			}

		private:
			std::tuple<TSigs...> _sigs;

			template<std::size_t I>
			el::enable_if_t<(I >= SignatureList::size), bool>
			compareImpl(Bitset const &othPrint) const noexcept
			{
				return true;
			}

			template<std::size_t I>
			el::enable_if_t<(I < SignatureList::size), bool>
			compareImpl(Bitset const &othPrint) const noexcept
			{
				return std::get<I>(this->_sigs).compare(othPrint)
					? this->compareImpl<I + 1>(othPrint)
					: false;
			}
		};

		template<typename ...TSigs>
		class SignatureOr {
		public:
			// static_assert(el::detail::_and<

			// 	>::value,
			// 	"Template arguments must all"
			// 	" implement the concept of Signature.");
			using SignatureList = el::type_list<TSigs...>;
			constexpr SignatureOr() = default;

			constexpr static std::size_t length() noexcept {
				return SignatureList::Current::List::size;
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
				if (I >= SignatureList::size)
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
			// 	return SignatureList::Current::List::size;
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
			// using SignatureList = el::type_list<TSigs...>;
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