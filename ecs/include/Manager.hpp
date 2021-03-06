/*
 * Manager.h
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#pragma once
#include <cstdlib>
#include <functional>
#include <tuple>
#include <vector>
#include <cassert>
#include "el/remove_cv.hpp"
#include "el/types/type_c.hpp"
#include "el/types/map.hpp"
#include "el/type_list/type_list.hpp"
#include "Settings.hpp"
#include "Entity.hpp"
#include "EntityHandle.hpp"
#include "Signature.hpp"

namespace ecs {
	struct HandleData {
		HandleData(ecs::EntityIdx entPos, int phase = 0):
		entityPosition(entPos), phase(phase)
		{
		}

		ecs::EntityIdx entityPosition;
		int phase;
	};

	template<typename TSettings>
	class Manager {
	public:
		using Settings = TSettings;
		using Entity = ecs::Entity<Settings>;
		using Handle = ecs::EntityHandle<Settings>;

		constexpr static std::size_t componentCount = Settings::ComponentList::size;
		constexpr static std::size_t tagCount = Settings::TagList::size;

		template<typename T>
		constexpr static bool isComponent = Settings::ComponentList::template Contains<T>::value;
		template<typename T>
		constexpr static bool isComponentConst = Settings::ComponentList::template Contains<T>;

		template<typename T>
		constexpr static std::size_t componentId = Settings::ComponentList::template IndexOf<T>::value;

		template<typename T>
		constexpr static bool isTag = Settings::TagList::template Contains<T>::value;

		template<typename T>
		constexpr static std::size_t tagId = Settings::TagList::template IndexOf<T>::value;

		Manager() {
			this->enlarge(1000);
		}

		std::size_t capacity() const noexcept { return this->_capacity; }
		/*
			* Vittorio Romeo had chosen for his entityCount that it would record
			* neither the entities destroyed between two calls to .refresh() as it
			* would require a specialized variable/logic, nor the entities created
			* during that time, for consistency with the previous choice.
			*
			* Here, I've chosen to count the entities created, to save myself the
			* iteration over the metadata of entities known to be absolutely dead
			* (even more critical after a long operation time, due to the allocation
			* done for reserve)
			* and to actually know how much metadata are we iterating over with
			* calls to .forEntities() and .forEntitiesMatching()
		*/
		std::size_t entityCount() const noexcept { return this->_size; }

		ecs::HandleData const &getHandleData(ecs::HandleDataIdx hdIdx) noexcept {
			return this->_handleData[hdIdx];
		}

		Entity const &getEntity(ecs::HandleDataIdx dataIdx) const noexcept {
			// std::cout << "GetConstantEntity" << std::endl;
			return this->_entities[this->_handleData[dataIdx].entityPosition];
		}

		Entity &getEntity(ecs::HandleDataIdx dataIdx) noexcept {
			// std::cout << "GetEntity" << std::endl;
			return this->_entities[this->_handleData[dataIdx].entityPosition];
		}

		void enlarge(std::size_t addition) {
			auto &entitiesSto = this->_entities;
			auto &hdSto = this->_handleData;
			auto capa = this->_capacity;
			auto nCapa = capa + (1 + addition / 128u) * 128;

			Settings::ComponentList::for_each([this, nCapa](auto, auto id) {
				static_assert(el::is_same_v<decltype(id()), size_t>);
				std::get<id()>(this->_componentStorage).reserve(nCapa);
			});
			entitiesSto.reserve(nCapa);
			hdSto.reserve(nCapa);
			for (auto hdIdx = capa; hdIdx < nCapa; ++hdIdx) {
				entitiesSto.emplace_back(hdIdx, hdIdx);
				hdSto.emplace_back(hdIdx);
			}
			this->_capacity = nCapa;
		}

		auto createEntity() noexcept {
			auto size = this->_size;

			if (size >= this->_capacity)
				this->enlarge(size / 2);
			// ++this->_handleData[size].phase;
			auto h = Handle(*this, size, ++this->_handleData[size].phase);
			++this->_size;
			return h;
		}

		void killEntity(ecs::HandleDataIdx dataIdx) noexcept {
			Settings::ComponentList::for_each([&](auto e, auto) {
				if (this->template hasComponent<TYPE_OF(e)>(dataIdx))
					this->template removeComponent<TYPE_OF(e)>(dataIdx);
			});
			this->_entities[this->_handleData[dataIdx].entityPosition].kill();
		}

		void refresh() noexcept {
			if (this->_size == 0)
				return;
			std::size_t iAlive = this->_size - 1;
			std::size_t iDead = 0;
			auto &hds = this->_handleData;
			auto &ents = this->_entities;

			while (iDead < iAlive) {
				for(; ents[iDead].alive()/* && (iDead < iAlive)*/; ++iDead);
				for(; !ents[iAlive].alive()/* && (iDead < iAlive)*/; --iAlive);
				if (iDead >= iAlive)
					break;
				auto &d = ents[iDead];
				auto &a = ents[iAlive];

				std::swap(d, a);
				hds[d.hdIndex].entityPosition = iDead;
				// ++hdAlive.phase;
				hds[a.hdIndex].entityPosition = iAlive;
			}
			this->_size = iDead;
		}

		template<typename T>
		bool hasComponent(ecs::HandleDataIdx hdIdx) const noexcept {
			static_assert(isComponent<T>, "T isn't a Component according to the Settings.");
			auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

			assert(e.alive());
			return e.template hasComponent<T>();
		}

		template<typename T>
		T& getComponent(ecs::HandleDataIdx hdIdx) noexcept {
			static_assert(isComponent<T>, "T isn't a Component according to the Settings.");
			auto entityPosition = this->_handleData[hdIdx].entityPosition;
			auto &e = this->_entities[entityPosition];

			assert(e.alive());
			return std::get<componentId<T>>(this->_componentStorage)[e.eid];
		}

		template<typename T, typename ...Args>
		T& addComponent(ecs::HandleDataIdx hdIdx, Args&&... args) noexcept {
			static_assert(isComponent<T>, "T isn't a Component according to the Settings.");
			auto entityPosition = this->_handleData[hdIdx].entityPosition;
			auto &e = this->_entities[entityPosition];

			assert(e.alive());
			auto &c = std::get<componentId<T>>(this->_componentStorage)[e.eid];
			new (&c) T(std::forward<Args>(args)...);
			e.template addComponent<T>();
			return c;
		}

		template<typename T>
		void removeComponent(ecs::HandleDataIdx hdIdx) noexcept {
			static_assert(isComponent<T>, "T isn't a Component according to the Settings.");
			auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

			assert(e.alive());
			e.template removeComponent<T>();
			auto &c = std::get<componentId<T>>(this->_componentStorage)[e.eid];
			c.~T();
		}

		template<typename T>
		bool hasTag(ecs::HandleDataIdx hdIdx) const noexcept {
			static_assert(isTag<T>, "T isn't a Tag according to the Settings.");
			auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

			assert(e.alive());
			return e.template hasTag<T>();
		}

		template<typename T>
		void addTag(ecs::HandleDataIdx hdIdx) noexcept {
			static_assert(isTag<T>, "T isn't a Tag according to the Settings.");
			auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

			assert(e.alive());
			e.template addTag<T>();
		}

		template<typename T>
		void removeTag(ecs::HandleDataIdx hdIdx) noexcept {
			static_assert(isTag<T>, "T isn't a Tag according to the Settings.");
			auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

			assert(e.alive());
			e.template removeTag<T>();
		}

		template<typename F, typename ...Args>
		void forEntities(F&& f, Args&&... args) noexcept {
			auto size = this->_size;
			auto &hdt = this->_handleData;
			auto &et = this->_entities;

			for (std::size_t i = 0; i < size; ++i) {
				auto &e = et[i];
				if (e.alive()) {
					auto h = Handle(*this, e.hdIndex, hdt[e.hdIndex].phase);
					std::forward<F>(f)(h, std::forward<Args>(args)...);
				}
			}
		}

		template<typename TSig, typename F, typename ...Args>
		void forEntitiesMatching(F&& f, Args&&... args) noexcept {
			auto size = this->_size;
			auto &hdt = this->_handleData;
			auto &et = this->_entities;

			for (std::size_t i = 0; i < size; ++i) {
				auto &e = et[i];
				if (e.alive() && e.matchesSignature(TSig{})) {
					auto h = Handle(*this, e.hdIndex, hdt[e.hdIndex].phase);
					std::forward<F>(f)(h, std::forward<Args>(args)...);
				}
			}
		}

		template<typename TSig, typename F, typename ...Args>
		void forEntitiesMatching(TSig&& s, F&& f, Args&&... args) noexcept {
			auto size = this->_size;
			auto &hdt = this->_handleData;
			auto &et = this->_entities;

			for (std::size_t i = 0; i < size; ++i) {
				auto &e = et[i];
				if (e.alive() && e.matchesSignature(s)) {
					auto h = Handle(*this, e.hdIndex, hdt[e.hdIndex].phase);
					std::forward<F>(f)(h, std::forward<Args>(args)...);
				}
			}
		}

	private:
		template<typename ...Types>
		using ComponentStorageTuple = std::tuple<std::vector<Types>...>;
		using ComponentStorage = el::Rename<ComponentStorageTuple, typename Settings::ComponentList>;

		std::size_t _size = 0;
		std::size_t _capacity = 0;
		std::vector<ecs::HandleData> _handleData;
		std::vector<ecs::Entity<Settings>> _entities;
		ComponentStorage _componentStorage;
	};
} // ecs