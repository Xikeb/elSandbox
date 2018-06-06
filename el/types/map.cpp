#ifndef ELMETA_MAP_HPP
	#define ELMETA_MAP_HPP
	#include <utility>
	#include "el/is_same.hpp"
	#include "el/enable_if.hpp"
	#include "el/conditional.hpp"
	#include "el/types/integral_c.hpp"
	#include "el/types/type_c.hpp"
	#include "el/types/nothing.hpp"
	#include "el/types/pair.hpp"

	namespace el {
		template<typename ...Pairs>
		struct map;

		namespace impl {
			template<typename T>
			constexpr auto is_pair(T&&) noexcept
			{
				return el::false_c{};
			}

			template<typename TFirst, typename TSecond>
			constexpr auto is_pair(el::Type_c<el::pair<TFirst, TSecond>>) noexcept
			{
				return el::true_c{};
			}

			constexpr auto andf() {
				return el::true_c{};
			}

			template<bool ...Values>
			constexpr auto andf(el::integral_c<bool, true>, el::integral_c<bool, Values>... rest) {
				return el::impl::andf(rest...);
			}

			template<bool ...Values>
			constexpr auto andf(el::integral_c<bool, false>, el::integral_c<bool, Values>... rest) {
				return el::false_c{};
			}

			template<typename Key>
			constexpr auto count(el::Type_c<Key> const &)
			{
				return 0;
			}

			template<typename Key, typename HKey, typename HValue, typename ...Keys, typename ...Values>
			constexpr auto count(
				el::Type_c<Key> const &me,
				el::Type_c<el::pair<HKey, HValue>> const &,
				el::Type_c<el::pair<Keys, Values>> const &... rest
			) noexcept
			{
				return (el::is_same<Key, HKey>::value ? 1 : 0) + count(me, rest...);
			}

			template<typename Key, typename HValue, typename ...Keys, typename ...Values>
			constexpr auto get_pair_by_key(
				el::Type_c<Key> me,
				el::Type_c<el::pair<Key, HValue>> head,
				el::Type_c<el::pair<Keys, Values>> ... rest
			) noexcept
			{
				return head;
			}

			template<typename Key>
			constexpr auto get_pair_by_key(
				el::Type_c<Key>
			) noexcept
			{
				return el::type_c<void>;
			}

			template<typename Key, typename NKey, typename NValue,
				typename ...Pairs, typename HValue,
				typename ...Keys, typename ...Values>
			constexpr auto replace_key(
				el::Type_c<el::map<Pairs...>>,
				el::Type_c<el::map<el::pair<Key, HValue>, el::pair<Keys, Values>...>>,
				el::Type_c<Key>,
				el::Type_c<el::pair<NKey, NValue>>,
				int
			) {
				return el::type_c<el::map<
					Pairs...,
					el::pair<NKey, NValue>,
					el::pair<Keys, Values>...
				>>;
			}

			template<typename Key, typename NKey, typename NValue,
				typename ...Pairs, typename HKey, typename HValue,
				typename ...Keys, typename ...Values>
			constexpr auto replace_key(
				el::Type_c<el::map<Pairs...>>,
				el::Type_c<el::map<el::pair<HKey, HValue>, el::pair<Keys, Values>...>>,
				el::Type_c<Key> from,
				el::Type_c<el::pair<NKey, NValue>> to,
				...
			) {
				return el::impl::replace_key(
					el::type_c<el::map<Pairs..., el::pair<HKey, HValue>>>,
					el::type_c<el::map<el::pair<Keys, Values>...>>,
					from,
					to,
					int{}
				);
			}

			template<typename TFirst, typename TSecond>
			struct map_element {
				constexpr map_element()
				{
				}

				constexpr map_element(el::pair<TFirst, TSecond> const &a): p(a)
				{
				}

				constexpr map_element(el::pair<TFirst, TSecond> &&a): p(std::move(a))
				{
				}

				el::pair<TFirst, TSecond> p;
			};
		} // impl

		template<typename ...Keys, typename ...Values>
		struct map<el::pair<Keys, Values>...>: public el::impl::map_element<Keys, Values>... {
			static_assert(
				el::impl::andf(
					el::bool_c<
						(el::impl::count(el::type_c<Keys>, el::type_c<el::pair<Keys, Values>>) == 1)
					>{}...
				)(),
				"Each key must only appear once");

			template<typename ...OthPairs, typename = el::enable_if_t<
				sizeof...(OthPairs) == sizeof...(Keys)
				&& el::impl::andf(el::impl::is_pair(el::type_c<OthPairs>)...)(),
				void
			>>
			explicit map(OthPairs&&... othPairs):
			el::impl::map_element<Keys, Values>(std::forward<OthPairs>(othPairs))...
			{
			}

			constexpr map()
			{
			}

			template<typename Key>
			constexpr auto &get(el::Type_c<Key> const &key) noexcept
			{
				using Pair = typename decltype(el::impl::get_pair_by_key(key, el::type_c<el::pair<Keys, Values>>...))::type;
				static_assert(!el::is_same<Pair, void>::value, "Map has no such key");
				el::impl::map_element<typename Pair::First, typename Pair::Second> &self = *this;
				return self.p;
			}

			template<typename Key>
			constexpr auto &operator[](el::Type_c<Key> const &key) noexcept
			{
				return this->get(key);
			}

			template<typename Key, typename NKey, typename NValue>
			using ReplacePair = el::type_of<decltype(
				el::impl::replace_key(
					el::type_c<el::map<>>,
					el::type_c<el::map<el::pair<Keys, Values>...>>,
					el::type_c<Key>,
					el::type_c<el::pair<NKey, NValue>>,
					int{}
				)
			)>;

			template<typename Key, typename NKey, typename NValue>
			constexpr auto replace_pair(el::Type_c<Key>, el::Type_c<el::pair<NKey, NValue>>) noexcept
			{
				/*using TYPE_OF(el::impl::replace_key(
					el::type_c<el::map<>>,
					el::type_c<el::map<el::pair<Keys, Values>...>>,
					from,
					to
				))();*/
				return ReplacePair<Key, NKey, NValue>();
			}
		};

		template<typename ...Keys, typename ...Values>
		constexpr auto make_map(el::pair<Keys, Values>&&... pairs) noexcept
		{
			return el::map<el::pair<Keys, Values>...>(std::forward<el::pair<Keys, Values>>(pairs)...);
		}
	} // el

	/*#include <iostream>
	using namespace std;
	template<typename T>
	void pretty_print(T t __attribute__((unused))) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
	}

	int main(int argc, char **argv)
	{
		auto m = el::make_map(
			el::make_pair(el::int_c<0>{}, el::true_c{}),
			el::make_pair(el::int_c<1>{}, el::false_c{}),
			el::make_pair(el::int_c<2>{}, el::true_c{}),
			el::make_pair(el::int_c<3>{}, el::false_c{})
		);
		pretty_print(m.get(el::type_c<el::int_c<0>>));
		cout << "Before: "; pretty_print(m);
		cout << "After: "; pretty_print(m.replace_pair(
			el::type_c<
				el::int_c<1>
			>,
			el::type_c<
				el::pair<
					el::short_c<9>,
					el::short_c<10>
				>
			>
		));
		return 0;
	}*/
#endif // ELMETA_MAP_HPP