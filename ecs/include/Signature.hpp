#pragma once
#include <cassert>
#include <cstdlib>
#include <bitset>
#include <tuple>
#include <iostream>
#include <type_traits>

#include "el/detail/and.hpp"
#include "el/type_list/type_list.hpp"
#include "el/types/type_c.hpp"

#include "Settings.hpp"
#include "SignatureBitset.hpp"
#include "SignatureConcept.hpp"
#include "Settings.hpp"

namespace ecs {
	namespace impl {
		template<typename EnabledTypesList, typename ...AllTypes>
		constexpr static char bitsetCode[sizeof...(AllTypes) + 1] = {
			(EnabledTypesList::template Contains<AllTypes>::value ? '0' : '1')...,
			'\0'
		};
	} // impl

	/**
	 * original version, before the tests with the value semantics version type_list
	 */
	// template<typename TSettings, typename ...TTypes>
	// class Signature {
	// public:
	// 	using Settings = ecs::detail::get_basic_settings<TSettings>;
	// 	using Types = el::type_list_t<TTypes...>;
	// 	/*using Components = el::type_of<decltype(Types::filter(Settings::ComponentList::has))>;
	// 	using Tags = el::type_of<decltype(Types::filter(Settings::TagList::has))>;*/
	// 	using Components = typename Types::template Filter<typename Settings::ComponentList::Has>;
	// 	using Tags = typename Types::template Filter<typename Settings::TagList::Has>;
	// 	using Required = Types;

	// 	constexpr Signature()
	// 	{
	// 		Components::for_each([&](auto &&e, auto &&) {
	// 			this->_sto.template enableComponent<TYPE_OF(e)>();
	// 		});
	// 		Tags::for_each([&](auto &&e, auto &&) {
	// 			this->_sto.template enableTag<TYPE_OF(e)>();
	// 		});
	// 	}

	// 	constexpr static std::size_t componentCount = Components::size;
	// 	constexpr static std::size_t tagCount = Tags::size;

	// 	template<typename OthSettings>
	// 	bool compare(SignatureBitset<OthSettings> const &othPrint) const noexcept
	// 	{
	// 		return this->_sto.matches(othPrint);
	// 	}

	// 	SignatureBitset<Settings> getBitset() const noexcept { return this->_sto; }
	// private:
	// 	SignatureBitset<Settings> _sto;
	// };

	template<typename TSettings, typename ...TTypes>
	class Signature {
	public:
		using Settings = ecs::detail::get_basic_settings<TSettings>;
		constexpr static auto types = el::type_list_t<TTypes...>{};
		
		constexpr static auto componentList = typename Settings::ComponentList{};
		constexpr static auto ownComponents = types.filter([](auto t) { return componentList.contains(t); });
		constexpr static std::size_t componentCount = ownComponents.size;

		constexpr static auto tagList = typename Settings::TagList{};
		constexpr static auto ownTags = types.filter([](auto t) { return tagList.contains(t); });
		constexpr static std::size_t tagCount = ownTags.size;

		constexpr Signature() {
			ownComponents.for_each([this](auto e, auto) { this->_sto.enableComponent(e); });
			ownTags.for_each([this](auto e, auto) { this->_sto.enableTag(e); });
		}

		template<typename OthSettings>
		bool compare(SignatureBitset<OthSettings> const &othPrint) const noexcept {
			return this->_sto.matches(othPrint);
		}

		SignatureBitset<Settings> getBitset() const noexcept { return this->_sto; }
	private:
		SignatureBitset<Settings> _sto;
	};

	template<typename TSettings, typename ...TTypes>
	constexpr static ecs::Signature<TSettings, TTypes...> signature{};

	class SignatureTrue {
	public:
		template<typename TSettings>
		bool compare(SignatureBitset<TSettings> const &) const noexcept
		{
			return true;
		}
	};
	constexpr static ecs::SignatureTrue signatureTrue{};

	template<typename ...TSigs>
	class SignatureAnd {
	public:
		static_assert(el::detail::andf(ecs::isSignature(TSigs())...),
			"Operands of signature logic classes must all "
			"implement the concept of Signature.");

		using SignatureList = el::type_list_t<TSigs...>;
		// using Settings = typename SignatureList::First::Settings;
		constexpr SignatureAnd() = default;

		constexpr SignatureAnd(TSigs&&... sigs): _sigs(sigs...)
		{
		}

		constexpr static std::size_t length = SignatureList::Current::List::size;

		template<typename OthSettings>
		bool compare(SignatureBitset<OthSettings> const &othPrint) const
		{
			return this->compareImpl<0>(othPrint);
		}

		el::enable_if_t<
			(SignatureList::size > 0) && el::detail::andf(ecs::canGetBitset<TSigs>()...),
			SignatureBitset<typename SignatureList::First::Settings>
		>
		getBitset() const noexcept
		{ return this->_sto; }
	private:
		std::tuple<TSigs...> _sigs;

		template<typename OthSettings, std::size_t I>
		el::enable_if_t<(I >= SignatureList::size), bool>
		compareImpl(SignatureBitset<OthSettings> const &) const noexcept
		{
			return true;
		}

		template<typename OthSettings, std::size_t I>
		el::enable_if_t<(I < SignatureList::size), bool>
		compareImpl(SignatureBitset<OthSettings> const &othPrint) const noexcept
		{
			return std::get<I>(this->_sigs).compare(othPrint)
				? this->compareImpl<I + 1>(othPrint)
				: false;
		}
	};

	template<typename ...TSigs>
	class SignatureOr {
	public:
		static_assert(el::detail::andf(ecs::isSignature(TSigs())...),
			"Operands of signature logic classes must all "
			"implement the concept of Signature.");

		using SignatureList = el::type_list_t<TSigs...>;
		constexpr SignatureOr() = default;

		constexpr SignatureOr(TSigs&&... sigs): _sigs(sigs...)
		{
		}

		template<typename OthSettings>
		bool compare(SignatureBitset<OthSettings> const &othPrint) const
		{
			return this->compareImpl<0>(othPrint);
		}

	private:
		std::tuple<TSigs...> _sigs;

		template<typename OthSettings, std::size_t I>
		el::enable_if_t<(I >= SignatureList::size), bool>
		compareImpl(SignatureBitset<OthSettings> const &) const noexcept
		{
			return false;
		}

		template<typename OthSettings, std::size_t I>
		el::enable_if_t<(I < SignatureList::size), bool>
		compareImpl(SignatureBitset<OthSettings> const &othPrint) const noexcept
		{
			return std::get<I>(this->_sigs).compare(othPrint)
				? true
				: this->compareImpl<I + 1>(othPrint);
		}
	};

	template<typename TSig>
	class SignatureNot: TSig {
	public:
		static_assert(decltype(ecs::isSignature(TSig()))::value,
			"Operands of signature logic classes must all "
			"implement the concept of Signature.");
		constexpr SignatureNot() = default;

		constexpr SignatureNot(TSig&& sig): _sig(sig)
		{
		}

		using Signature = TSig;
		using Settings = typename Signature::Settings;

		template<typename OthSettings>
		bool compare(SignatureBitset<OthSettings> const &othPrint) const
		{
			return !this->TSig::compare(othPrint);
		}

	private:
		Signature _sig;
	};

	template<typename TSigCond, typename TSigTrue, typename TSigFalse>
	class SignatureConditional {
	public:
		static_assert(el::detail::andf(
				ecs::isSignature(TSigCond())(),
				ecs::isSignature(TSigTrue())(),
				ecs::isSignature(TSigFalse())()
			),
			"Operands of signature logic classes must all "
			"implement the concept of Signature.");
		using Condition = TSigCond;
		using IfSignature = TSigTrue;
		using ElseSignature = TSigFalse;
		constexpr SignatureConditional() = default;

		constexpr SignatureConditional(
			Condition const &sCond,
			IfSignature const &sTrue,
			ElseSignature const &sFalse
		): _cond(sCond), _if(sTrue), _else(sFalse)
		{
		}

		template<typename OthSettings>
		bool compare(SignatureBitset<OthSettings> const &othPrint) const
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

	template<typename ...T1Sigs, typename ...T2Sigs>
	auto operator&(
		ecs::SignatureAnd<T1Sigs...> const&,
		ecs::SignatureAnd<T2Sigs...> const&
	) noexcept
	{
		return ecs::SignatureAnd<T1Sigs..., T2Sigs...>{};
	}

	template<typename ...T1Sigs, typename T2Sig>
	el::enable_if_t<
		decltype(ecs::isSignature(T2Sig{}))::value,
		ecs::SignatureAnd<std::decay_t<T1Sigs>..., std::decay_t<T2Sig>>
	>
	operator&(
		ecs::SignatureAnd<T1Sigs...> const&,
		T2Sig const&
	) noexcept
	{
		return ecs::SignatureAnd<std::decay_t<T1Sigs>..., std::decay_t<T2Sig>>{};
	}

	template<typename ...T1Sigs, typename ...T2Sigs>
	auto operator|(
		ecs::SignatureOr<T1Sigs...> const&,
		ecs::SignatureOr<T2Sigs...> const&
	) noexcept
	{
		return ecs::SignatureOr<T1Sigs..., T2Sigs...>{};
	}

	template<typename ...T1Sigs, typename T2Sig>
	el::enable_if_t<
		decltype(ecs::isSignature(T2Sig{}))::value,
		ecs::SignatureOr<std::decay_t<T1Sigs>..., std::decay_t<T2Sig>>
	>
	operator|(
		ecs::SignatureOr<T1Sigs...> const&,
		T2Sig const&
	) noexcept
	{
		return ecs::SignatureOr<std::decay_t<T1Sigs>..., std::decay_t<T2Sig>>{};
	}
} // ecs