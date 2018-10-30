/*
 * Manager.h
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#ifndef MANAGER_H_
	#define MANAGER_H_
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
			HandleData()
			{
			}

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
			constexpr static std::size_t componentId = Settings::ComponentList::template IndexOf<T>::value;

			template<typename T>
			constexpr static bool isTag = Settings::TagList::template Contains<T>::value;

			template<typename T>
			constexpr static std::size_t tagId = Settings::TagList::template IndexOf<T>::value;

			Manager()
			{
				this->enlarge(1000);
			}

			ecs::HandleData const &getHandleData(ecs::HandleDataIdx hdIdx) noexcept
			{
				return this->_handleData[hdIdx];
			}

			Entity &getEntity(ecs::HandleDataIdx dataIdx) noexcept
			{
				return this->_entities[this->_handleData[dataIdx].entityPosition];
			}

			void enlarge(std::size_t addition)
			{
				auto capa = this->_capacity;
				auto &entitiesSto = this->_entities;
				auto &hdSto = this->_handleData;
				auto nCapa = capa + (1 + addition % 128) * 128;

				Settings::ComponentList::for_each([&](auto &, auto &i) {
					std::get<i()>(this->_componentStorage).reserve(nCapa);
				});
				entitiesSto.reserve(nCapa);
				hdSto.reserve(nCapa);
				for (auto hdIdx = capa; hdIdx < nCapa; ++hdIdx) {
					entitiesSto.emplace_back(hdIdx, hdIdx);
					hdSto.emplace_back(hdIdx);
				}
				this->_capacity = nCapa;
			}

			auto createEntity() noexcept
			{
				auto size = this->_size;

				if (size >= this->_capacity)
					this->enlarge(size / 2);
				auto h = Handle(*this, size, this->_handleData[size].phase);
				++this->_size;
				return h;
			}

			void killEntity(ecs::HandleDataIdx dataIdx) noexcept
			{
				Settings::ComponentList::for_each([&](auto &e, auto &) {
					if (this->template hasComponent<typename decltype(+e)::type>(dataIdx))
						this->template removeComponent<typename decltype(+e)::type>(dataIdx);
				});
				this->_entities[this->_handleData[dataIdx].entityPosition].kill();
			}

			void refresh() noexcept
			{
				auto itAlive = this->_size - 1;
				auto itDead = 0;
				auto &hds = this->_handleData;

				while (itDead < itAlive) {
					auto &d = this->_entities[itDead];
					auto &a = this->_entities[itAlive];
					for(; d->alive(); ++itDead);
					for(; !a->alive(); ++itAlive);
					if (itDead < itAlive) {
						auto &hdDead = hds[d.hdIndex];
						auto &hdAlive = hds[a.hdIndex];

						std::swap(d, a);
						hdDead.entityPosition = itAlive;
						++hdAlive.phase;
						hdAlive.entityPosition = itDead;
					} else
						break;
				}
				this->size = itDead;
			}

			template<typename T>
			el::enable_if_t<isComponent<T>, bool> hasComponent(ecs::HandleDataIdx hdIdx) const noexcept
			{
				auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

				assert(e.alive());
				return e.template hasComponent<T>();
			}

			template<typename T>
			el::enable_if_t<isComponent<T>, T&> getComponent(ecs::HandleDataIdx hdIdx) noexcept
			{
				auto entityPosition = this->_handleData[hdIdx].entityPosition;
				auto &e = this->_entities[entityPosition];

				assert(e.alive());
				return std::get<componentId<T>>(this->_componentStorage)[e.eid];
			}

			template<typename T, typename ...Args>
			el::enable_if_t<isComponent<T>, T&> addComponent(ecs::HandleDataIdx hdIdx, Args&&... args) noexcept
			{
				auto entityPosition = this->_handleData[hdIdx].entityPosition;
				auto &e = this->_entities[entityPosition];

				assert(e.alive());
				auto &c = std::get<componentId<T>>(this->_componentStorage)[e.eid];
				new (&c) T(std::forward<Args>(args)...);
				e.template addComponent<T>();
				return c;
			}

			template<typename T>
			el::enable_if_t<isComponent<T>, void> removeComponent(ecs::HandleDataIdx hdIdx) noexcept
			{
				auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

				assert(e.alive());
				e.template removeComponent<T>();
				auto &c = std::get<componentId<T>>(this->_componentStorage)[e.eid];
				c.~T();
			}

			template<typename T>
			el::enable_if_t<isTag<T>, bool> hasTag(ecs::HandleDataIdx hdIdx) const noexcept
			{
				auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

				assert(e.alive());
				return e.template hasTag<T>();
			}

			template<typename T>
			el::enable_if_t<isTag<T>, void> addTag(ecs::HandleDataIdx hdIdx) noexcept
			{
				auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

				assert(e.alive());
				e.template addTag<T>();
			}

			template<typename T>
			el::enable_if_t<isTag<T>, void> removeTag(ecs::HandleDataIdx hdIdx) noexcept
			{
				auto &e = this->_entities[this->_handleData[hdIdx].entityPosition];

				assert(e.alive());
				e.template removeTag<T>();
			}

			template<typename F, typename ...Args>
			void forEntities(F&& f, Args&&... args) noexcept
			{
				auto size = this->_size;
				auto &hdt = this->_handleData;
				auto &et = this->_entities;

				for (std::size_t i = 0; i < size; ++i) {
					auto &e = et[i];
					if (e.alive()) {
						auto h = Handle(*this, i, hdt[e.hdIndex].phase);
						f(h, std::forward<Args>(args)...);
					}
				}
			}

			template<typename TSig, typename F, typename ...Args>
			void forEntitiesMatching(F&& f, Args&&... args) noexcept
			{
				auto size = this->_size;
				auto &hdt = this->_handleData;
				auto &et = this->_entities;

				for (std::size_t i = 0; i < size; ++i) {
					auto &e = et[i];
					if (e.alive() && e.matchesSignature(TSig{})) {
						auto h = Handle(*this, i, hdt[e.hdIndex].phase);
						f(h, std::forward<Args>(args)...);
					}
				}
			}

			template<typename TSig, typename F, typename ...Args>
			void forEntitiesMatching(TSig&& s, F&& f, Args&&... args) noexcept
			{
				auto size = this->_size;
				auto &hdt = this->_handleData;
				auto &et = this->_entities;

				for (std::size_t i = 0; i < size; ++i) {
					auto &e = et[i];
					if (e.alive() && e.matchesSignature(s)) {
						auto h = Handle(*this, i, hdt[e.hdIndex].phase);
						f(h, std::forward<Args>(args)...);
					}
				}
			}

		private:
			template<typename ...Types>
			using ComponentStorage = std::tuple<std::vector<Types>...>;

			std::size_t _size = 0;
			std::size_t _capacity = 0;
			std::vector<ecs::HandleData> _handleData;
			std::vector<ecs::Entity<Settings>> _entities;
			el::Rename<ComponentStorage, typename Settings::ComponentList> _componentStorage;
		};
	} // ecs
#endif /* MANAGER_H_ */
