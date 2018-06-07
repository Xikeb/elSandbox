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
		namespace impl {
			struct SystemSettings {
				struct Dependencies;
				struct Instance;
				struct Signature;
				struct Execute;

				constexpr static auto &dependency = el::type_c<Dependencies>;
				constexpr static auto &instance = el::type_c<Instance>;
				constexpr static auto &signature = el::type_c<Signature>;
				constexpr static auto &execute = el::type_c<Execute>;
				struct Keys {
					using Dependencies = el::remove_cvref_t<decltype(dependency)>;
					using Instance = el::remove_cvref_t<decltype(instance)>;
					using Signature = el::remove_cvref_t<decltype(signature)>;
					using Execute = el::remove_cvref_t<decltype(execute)>;
				};
			};
		} // impl

		template<typename TSettings, typename TDependencyList, typename TInstance, typename TSignature, typename Fexecute>
		class System {
		public:
			using This = System<TSettings, TDependencyList, TInstance, TSignature, Fexecute>;
			using Dependencies = TDependencyList;
			using Instance = TInstance;
			using Signature = TSignature;
			using Execute = Fexecute;
			using Manager = ecs::Manager<TSettings>;

			System(This const &oth):
			inst(oth.inst), _mgr(oth.mgr), _f(oth._f)
			{
			}

			System(This &&oth):
			inst(std::move(oth.inst)), _mgr(oth.mgr), _f(std::move(oth._f))
			{
			}

			template<typename ...Args>
			System(Manager &mgr, std::function<Execute> &&f, Args&&... args):
			inst(std::forward<Args>(args)...), _mgr(mgr), _f(std::move(f))
			{
			}

			System(Manager &mgr, std::function<Execute> &&f, Instance &&i):
			inst(std::move(i)), _mgr(mgr), _f(std::move(f))
			{
			}

			System(Manager &mgr, std::function<Execute> &&f, Instance const &i):
			inst(i), _mgr(mgr), _f(std::move(f))
			{
			}

			template<typename ...Args>
			void operator()(Args&&... args)
			{
				this->_mgr.forEntitiesMatching(this->sig, this->_f, std::forward<Args>(args)...);
			}


			Instance inst;
		private:
			Manager &_mgr;
			std::function<Execute> _f;
			Signature sig;
		};

		template<typename TDependencyList = el::type_list<>, typename TInstance = void, typename TSignature = el::type_list<>, typename Fexecute = void(void)>
		class SystemSetup {
		public:
			using Settings = ecs::impl::SystemSettings;
			using Dependencies = TDependencyList;
			using Instance = TInstance;
			using Signature = TSignature;
			using Execute = Fexecute;

			el::map<
				el::pair<typename Settings::Keys::Dependencies, el::Type_c<Dependencies>>,
				el::pair<typename Settings::Keys::Instance, el::Type_c<Instance>>,
				el::pair<typename Settings::Keys::Signature, el::Type_c<Signature>>,
				el::pair<typename Settings::Keys::Execute, std::function<Execute>>
			> options;

			constexpr SystemSetup(std::function<Execute> &&f = nullptr):
			options(
				el::make_pair(Settings::dependency, el::type_c<Dependencies>),
				el::make_pair(Settings::instance, el::type_c<Instance>),
				el::make_pair(Settings::signature, el::type_c<Signature>),
				el::make_pair(Settings::execute, f)
			)
			{
			}

			~SystemSetup()
			{
			}

			template<typename T>
			constexpr auto instantiateWith(el::Type_c<T>) noexcept
			{
				static_assert(!el::is_same<T, Instance>::value, "System is already instantiated with this type.");
				return SystemSetup<Dependencies, T, Signature, Execute>(this->options[Settings::execute]);
			}

			template<typename F>
			constexpr auto execution(F&& f) noexcept
			{
				return SystemSetup<Dependencies, F, Signature, Execute>(f);
			}

			template<typename ...Requirements>
			constexpr auto dependOn(el::Type_c<el::type_list<Requirements...>>) noexcept
			{
				return SystemSetup<el::type_list<Requirements...>, Instance, Signature, Execute>(options[Settings::execute]);
			}

			template<typename TNewSignature>
			constexpr auto matching(el::Type_c<TNewSignature>) noexcept
			{
				return SystemSetup<Dependencies, Instance, TNewSignature, Execute>(options[Settings::execute]);
			}

			template<typename TSettings, typename ...Args>
			constexpr auto operator()(ecs::Manager<TSettings> &mgr, Args&&... args) noexcept
			{
				return System<TSettings, Dependencies, Instance, Signature, Execute>(
					mgr, std::move(this->options[Settings::execute]),
					std::forward<Args>(args)...
				);
			}
		};

		struct HandleData {
			ecs::EntityIdx entityPosition;
			int phase;
		};

		template<typename TSettings>
		class Manager {
		public:
			using Settings = TSettings;
			using ComponentList = typename Settings::ComponentList;
			using TagList = typename Settings::TagList;
			using Entity = ecs::Entity<Settings>;
			using Handle = ecs::EntityHandle<Settings>;

			constexpr static std::size_t componentCount = ComponentList::size;
			constexpr static std::size_t tagCount = TagList::size;

			template<typename T>
			constexpr static bool isComponent = ComponentList::template Contains<T>::value;

			template<typename T>
			constexpr static std::size_t componentId = ComponentList::template IndexOf<T>::value;

			template<typename T>
			constexpr static bool isTag = TagList::template Contains<T>::value;

			template<typename T>
			constexpr static std::size_t tagId = TagList::template IndexOf<T>::value;

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

				ComponentList().for_each([&](auto &, auto &i) {
					std::get<i()>(this->_componentStorage).reserve(nCapa);
				});
				entitiesSto.reserve(nCapa);
				hdSto.reserve(nCapa);
				for (auto hdIdx = capa; hdIdx < nCapa; ++hdIdx) {
					new (&entitiesSto[hdIdx]) Entity(hdIdx, hdIdx);
					auto &hd = hdSto[hdIdx];
					hd.entityPosition = hdIdx;
					hd.phase = 0;
				}
				this->_capacity = nCapa;
			}

			auto createEntity() noexcept
			{
				auto size = this->_size;

				if (size >= this->_capacity)
					this->enlarge(size / 2);
				++this->_size;
				return Handle(*this, this->_size, this->_handleData[this->_size].phase);
			}

			void refresh() noexcept
			{
				auto itAlive = this->_size - 1;
				auto itDead = 0;
				auto &hds = this->_handleData;

				while (itDead < itAlive) {
					auto &d = this->_entities[itDead];
					auto &a = this->_entities[itAlive];
					for(; (itDead < itAlive) && !d->alive(); ++itDead);
					for(; (itDead < itAlive) && a->alive(); ++itAlive);
					if (!d->alive() && a->alive()) {
						std::swap(d, a);
						hds[d.hdIndex].entityPosition = itAlive;
						++hds[d.hdIndex].phase;
						hds[a.hdIndex].entityPosition = itDead;
					}
				}
				this->size = std::distance(itDead, std::begin(this->_entities));
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
			el::Rename<ComponentStorage, ComponentList> _componentStorage;
		};
	} // ecs
#endif /* MANAGER_H_ */
