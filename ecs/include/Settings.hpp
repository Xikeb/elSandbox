#pragma once
#include "el/is_same.hpp"
#include "el/type_list/type_list.hpp"

namespace ecs {
	template<typename ...Types>
	using ComponentList = el::type_list<Types...>;
	template<typename ...Types>
	using TagList = el::type_list<Types...>;

	template<typename TComponentList, typename TTagList>
	struct BasicSettings {
		using ComponentList = TComponentList;
		using TagList = TTagList;
		template<typename T>
		using isComponent = typename ComponentList::template Contains<T>;
		template<typename T>
		using isTag = typename TagList::template Contains<T>;
	};

	template<typename TComponentList, typename TTagList>
	struct Settings {
		using ComponentList = TComponentList;
		using TagList = TTagList;
		using Basic = BasicSettings<ComponentList, TagList>;
	};

	namespace impl {
		// template<typename T> struct is_settings;
		template<typename T> struct is_basic_settings;

		// template<typename T>
		// struct is_settings: el::false_c
		// {
		// };

		// template<typename TComponentList, typename TTagList>
		// struct is_settings<ecs::Settings<TComponentList, TTagList>>: el::true_c
		// {
		// };

		template<typename T>
		using is_settings = el::is_similar<ecs::Settings, T>;
		template<typename T>
		constexpr static auto is_settings_v = is_settings<T>::value;

		template<typename T>
		struct is_basic_settings: el::false_c
		{
		};

		template<typename TComponentList, typename TTagList>
		struct is_basic_settings<ecs::BasicSettings<TComponentList, TTagList>>: el::true_c
		{
		};

		template<bool IsSettings, typename T>
		struct get_basic_settings;

		template<typename T>
		struct get_basic_settings<true, T> {using type = typename T::Basic;};
		template<typename T>
		struct get_basic_settings<false, T> {
			static_assert(ecs::impl::is_basic_settings<T>::value, "Self is neither full nor basic settings!");
			using type = el::enable_if_t<ecs::impl::is_basic_settings<T>::value, T>;
		};
	} // impl

	namespace detail {
		template<typename T>
		constexpr static bool is_settings = ecs::impl::is_settings<T>::value;
		template<typename T>
		constexpr static bool is_basic_settings = ecs::impl::is_basic_settings<T>::value;

		template<typename T>
		using get_basic_settings = typename ecs::impl::get_basic_settings<ecs::detail::is_settings<T>, T>::type;
	} // detail
} // ecs