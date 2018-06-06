#ifndef ECS_SETTINGS_HPP
	#define ECS_SETTINGS_HPP
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
	} // ecs
#endif // ECS_SETTINGS_HPP