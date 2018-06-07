#ifndef ELECS_SIGNATURE_HPP
	#define ELECS_SIGNATURE_HPP
	#include <cstdlib>
	#include <bitset>
	#include <tuple>
	#include <iostream>
	#include "el/detail/and.hpp"
	#include "Settings.hpp"
	#include "el/type_list/type_list.hpp"
	#include "el/types/is_valid.hpp"
	#include "el/types/type_c.hpp"
	
	namespace ecs {
		template<typename TSettings>
		class SignatureBitset {
		public:
			using This = SignatureBitset<TSettings>;
			using Settings = TSettings;
			using ComponentList = typename Settings::ComponentList;
			using TagList = typename Settings::TagList;

			constexpr SignatureBitset(): _bitset(0)
			{
			}

			SignatureBitset(const char bits[]): _bitset(bits)
			{
			}

			SignatureBitset(SignatureBitset const &oth):
			_bitset(oth._bitset)
			{
			}

			SignatureBitset &operator=(SignatureBitset const &rhs) noexcept
			{
				this->_bitset = rhs._bitset;
				return *this;
			}

			This &reset() noexcept
			{
				this->_bitset.reset();
				return *this;
			}

			constexpr static std::size_t componentCount = ComponentList::size;

			template<typename T>
			constexpr static bool isComponent = ComponentList::template Contains<T>::value;

			template<typename T>
			constexpr static bool componentId = ComponentList::template IndexOf<T>::value;

			template<typename T>
			constexpr static std::size_t componentBit = ComponentList::template IndexOf<T>::value;

			constexpr static std::size_t tagCount = TagList::size;

			template<typename T>
			constexpr static bool isTag = TagList::template Contains<T>::value;

			template<typename T>
			constexpr static bool tagId = TagList::template IndexOf<T>::value;

			template<typename T>
			constexpr static std::size_t tagBit = componentCount + TagList::template IndexOf<T>::value;

			template<typename T>
			el::enable_if_t<isComponent<T>, bool> hasComponent() const noexcept
			{
				return this->_bitset[componentBit<T>];
			}

			template<typename T>
			el::enable_if_t<isComponent<T>, SignatureBitset &> enableComponent() noexcept
			{
				this->_bitset.set(componentBit<T>);
				return *this;
			}

			template<typename T>
			el::enable_if_t<isComponent<T>, SignatureBitset &> disableComponent() noexcept
			{
				this->_bitset.reset(componentBit<T>);
				return *this;
			}

			template<typename T>
			el::enable_if_t<isComponent<T>, SignatureBitset &> toggleComponent() noexcept
			{
				this->_bitset.flip(componentBit<T>);
				return *this;
			}

			template<typename T>
			el::enable_if_t<isTag<T>, bool> hasTag() const noexcept
			{
				return this->_bitset[tagBit<T>];
			}

			template<typename T>
			el::enable_if_t<isTag<T>, SignatureBitset &> enableTag() noexcept
			{
				this->_bitset.set(tagBit<T>);
				return *this;
			}

			template<typename T>
			el::enable_if_t<isTag<T>, SignatureBitset &> disableTag() noexcept
			{
				this->_bitset.reset(tagBit<T>);
				return *this;
			}

			template<typename T>
			el::enable_if_t<isTag<T>, SignatureBitset &> toggleTag() noexcept
			{
				this->_bitset.flip(tagBit<T>);
				return *this;
			}

			bool matches(SignatureBitset<TSettings> const &oth) const noexcept
			{
				return (this->_bitset & oth._bitset) == this->_bitset;
			}

		private:
			std::bitset<componentCount + tagCount> _bitset;
		};

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

		template<typename TSettings, typename ...TTypes>
		class Signature {
		public:
			using Settings = TSettings;
			using ComponentList = typename Settings::ComponentList;
			using TagList = typename Settings::TagList;
			using Types = el::type_list<TTypes...>;
			using Components = el::type_of<decltype(ComponentList().filter(ComponentList::has))>;
			using Tags = el::type_of<decltype(TagList().filter(TagList::has))>;
			using Required = Types;

			constexpr Signature()/*: Signature(
				std::make_index_sequence<ComponentList::size>(),
				std::make_index_sequence<TagList::size>()
			)*/
			{
			}

			constexpr static std::size_t componentCount = ComponentList::size;
			constexpr static std::size_t tagCount = TagList::size;

			bool compare(SignatureBitset<Settings> const &othPrint) const
			{
				return this->_sto.matches(othPrint);
			}
		private:
			SignatureBitset<Settings> _sto;

			/*template<std::size_t ...CIdxs, std::size_t ...TIdxs>
			Signature(std::index_sequence<CIdxs...>, std::index_sequence<TIdxs...>):
			_sto(
				(char[]){
					el::conditional<
						Components::template Contains<
							typename ComponentList::template At<CIdxs>
						>::value,
						el::char_c<'0'>, el::char_c<'1'>
					>::type::value...,
					el::conditional<
						Tags::template Contains<
							typename ComponentList::template At<TIdxs>
						>::value,
						el::char_c<'0'>, el::char_c<'1'>
					>::type::value...
				}
			)
			{
			}*/
		};

		template<typename T>
		void pretty_print(T t __attribute__((unused))) {
			std::cout << __PRETTY_FUNCTION__ << std::endl;
		}

		namespace test {
			using Components = el::type_list<float, double>;
			using Tags = el::type_list<char>;
			using Settings = ecs::Settings<Components, Tags>;
			using IsLong = ecs::Signature<Settings::Basic, char>;
			static IsLong sig{};
		} // test

		template<typename ...TSigs>
		class SignatureAnd {
		public:
			static_assert(el::detail::_and<
					isSignature(TSigs())...
				>::value,
				"Template arguments must all"
				" implement the concept of Signature.");
			using SignatureList = el::type_list<TSigs...>;
			constexpr SignatureAnd() = default;

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