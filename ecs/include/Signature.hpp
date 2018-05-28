#ifndef ELECS_SIGNATURE_HPP
	#define ELECS_SIGNATURE_HPP
	#include <cstdlib>
	#include <bitset>
	#include <tuple>
	#include "el/detail/and.hpp"
	#include "el/type_list/type_list.hpp"
	#include "el/types/is_valid.hpp"
	
	namespace ecs {
		template<typename TSettings>
		class SignatureStorage {
		public:
			using Settings = TSettings;
			using Components = typename Settings::Components;
			using Tags = typename Settings::Tags;

			constexpr SignatureStorage(const char components[], const char tags[]):
			_componentBits(components), _tagBits(tags)
			{
			}

			constexpr static std::size_t componentsCount() noexcept
			{
				return Components::size;
			}

			constexpr static std::size_t tagsCount() noexcept
			{
				return Tags::size;
			}

			template<typename T>
			bool hasComponent() const noexcept
			{
				using Index = typename Components::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Component not part of this signature's settings.");
				return this->_componentBits[Index::value];
			}

			template<typename T>
			SignatureStorage &enableComponent() noexcept
			{
				using Index = typename Components::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Component not part of this signature's settings.");
				this->_componentBits[Index::value] = true;
				return *this;
			}

			template<typename T>
			SignatureStorage &disableComponent() noexcept
			{
				using Index = typename Components::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Component not part of this signature's settings.");
				this->_componentBits[Index::value] = false;
				return *this;
			}

			template<typename T>
			SignatureStorage &toggleComponent() noexcept
			{
				using Index = typename Components::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Component not part of this signature's settings.");
				this->_componentBits[Index::value] = !this->_componentBits[Index::value];
				return *this;
			}

			template<typename T>
			bool hasTag() const noexcept
			{
				using Index = typename Components::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Tag not part of this signature's settings.");
				return this->_tagBits[Index::value];
			}

			template<typename T>
			SignatureStorage &enableTag() noexcept
			{
				using Index = typename Tags::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Tag not part of this signature's settings.");
				this->_tagBits[Index::value] = true;
				return *this;
			}

			template<typename T>
			SignatureStorage &disableTag() noexcept
			{
				using Index = typename Tags::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Tag not part of this signature's settings.");
				this->_tagBits[Index::value] = false;
				return *this;
			}

			template<typename T>
			SignatureStorage &toggleTag() noexcept
			{
				using Index = typename Tags::template IndexOf<T>;
				static_assert(el::is_same<typename Index::type, bool>::value,
					"Tag not part of this signature's settings.");
				this->_tagBits[Index::value] = !this->_tagBits[Index::value];
				return *this;
			}

			auto &componentBits() noexcept
			{
				return this->_componentBits;
			}

			auto const &componentBits() const noexcept
			{
				return this->_componentBits;
			}

			auto &tagBits() noexcept
			{
				return this->_tagBits;
			}

			auto const &tagBits() const noexcept
			{
				return this->_tagBits;
			}

			bool matches(SignatureStorage<TSettings> const &rhs) const noexcept
			{
				return ((this->_componentBits & rhs._componentBits)
					== this->_componentBits)
				&& ((this->_tagBits & rhs._tagBits) == this->_tagBits);
			}

		private:
			std::bitset<Components::size>	_componentBits;
			std::bitset<Tags::size>		_tagBits;
		};

		template<typename TSettings, typename TComponents, typename TTags>
		class Signature {
		public:
			using Settings = TSettings;
			using Components = typename Settings::Components;
			using Tags = typename Settings::Tags;

			constexpr Signature(): Signature(
				std::make_index_sequence<Components::size>(),
				std::make_index_sequence<Tags::size>()
			)
			{
			}

			constexpr static std::size_t componentsCount()
			{
				return Components::size;
			}

			constexpr static std::size_t tagsCount()
			{
				return Tags::size;
			}

			bool compare(SignatureStorage<TSettings> const &othPrint) const
			{
				return this->_sto.matches(othPrint);
			}
		private:
			SignatureStorage<Settings> _sto;

			template<std::size_t ...CIdxs, std::size_t ...TIdxs>
			Signature(std::index_sequence<CIdxs...>, std::index_sequence<TIdxs...>):
			_sto(
				(char[]){
					el::conditional<
						TComponents::template Contains<
							Components::template At<CIdxs>
						>::value,
						el::char_c<'0'>, el::char_c<'1'>
					>::type::value...
				},
				(char[]){
					el::conditional<
						TComponents::template Contains<
							Components::template At<TIdxs>
						>::value,
						el::char_c<'0'>, el::char_c<'1'>
					>::type::value...
				}
			)
			{
			}
		};

		template<typename TSettings>
		struct SignatureConcept {
			using Settings = TSettings;
			constexpr SignatureConcept() = default;

			constexpr static auto canCompare = el::is_valid([](auto &&sig) -> bool {
				return sig.compare(SignatureStorage<Settings>{});
			});

			template<typename TSig>
			constexpr auto operator()(TSig &&sig) {
				return canCompare(sig);
			}
		};

		template<typename TSettings>
		constexpr static ecs::SignatureConcept<TSettings> isSigFunctor{};

		template<typename TSettings, typename TSig>
		constexpr auto isSignature(TSig &&sig) noexcept {
			return isSigFunctor<TSettings>(std::forward<TSig>(sig));
		}

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