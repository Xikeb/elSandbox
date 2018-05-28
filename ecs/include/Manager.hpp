/*
 * Manager.h
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#ifndef MANAGER_H_
	#define MANAGER_H_
	#include <cstdlib>
	#include <tuple>
	#include <vector>
	#include "el/type_list/type_list.h"

	namespace ecs {
		template<typename TSettings>
		class Manager {
		public:
			using TSetting = TSettings;
			using TComponentList = TSettings::Components;
			using TTagList = TSettings::Tags;

			using Eid = std::size_t;

			void enlarge(std::size_t addition);
			template<typename T>
			el::enable_if<
				el::Includes<TComponentList>::value,
				T &
			>::type getComponent(Manager::Eid eid)
			{
				if (!this->hasComponent<T>())
					throw std::runtime_error(
						"No such component."
					);
				return std::get<
					TComponentList::IndexOf<T>
				>(this->_componentStorage);
			}

		private:
			template<typename T>
			using TupleOfVectors = std::tuple<std::vector<T>>;

			std::vector<ecs::Entity<TSettings>> _entities;
			el::Rename<TupleOfVectors, TComponentList> _componentStorage;
			std::size_t _size;
			std::size_t _capacity;
		};
	} // ecs
#endif /* MANAGER_H_ */
