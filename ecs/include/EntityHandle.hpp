/*
 * EntityHandle.h
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#pragma once
#include <cassert>
#include "Entity.hpp"

namespace ecs {
	template<typename TSettings>
	class Manager;

	template<typename TSettings>
	class EntityHandle {
	public:
		using Settings = TSettings;
		using Self = ecs::EntityHandle<Settings>;
		using Manager = ecs::Manager<Settings>;

		constexpr static std::size_t componentCount = Manager::componentCount;
		constexpr static std::size_t tagCount = Manager::tagCount;

		template<typename T>
		constexpr static bool isComponent = Manager::template isComponent<T>;
		template<typename T>
		constexpr static std::size_t componentId = Manager::template componentId<T>;
		template<typename T>
		constexpr static bool isTag = Manager::template isTag<T>;
		template<typename T>
		constexpr static std::size_t tagId = Manager::template tagId<T>;

		explicit EntityHandle(Manager &mgr) noexcept:
		_mgr(mgr), _dataIdx(-1), _phase(-1)
		{
		}

		EntityHandle(
			Manager &mgr,
			ecs::HandleDataIdx dataIdx,
			int phase
		): _mgr(mgr), _dataIdx(dataIdx), _phase(phase)
		{
		}

		EntityHandle(Self const &oth):
		_mgr(oth._mgr), _dataIdx(oth._dataIdx),
		_phase(oth._phase)
		{
			// std::cerr << "Entity Handle was copied" << std::endl;
		}

		Self &operator=(Self const &oth) noexcept
		{
			//Placement-new is required because of reference to manager
			return *(new (this) Self(oth));
		}

		int getPhase() const noexcept { return this->_phase; }
		auto &getManager() noexcept { return this->_mgr; }

		bool isInPhase() const noexcept
		{
			return (this->_mgr.getHandleData(this->_dataIdx).phase == this->_phase);
		}

		bool isValid() const noexcept
		{
			return this->isInPhase() && this->_mgr.getEntity(this->_dataIdx);
		}

		void kill() noexcept
		{
			// auto &mgr = this->_mgr;
			// auto dataIdx = this->_dataIdx;

			// assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			assert(this->isInPhase());
			this->_mgr.killEntity(this->_dataIdx);
		}

		template<typename TSignature>
		bool matchesSignature(TSignature &&sig) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.getEntity(dataIdx).matchesSignature(std::forward<TSignature>(sig));
		}

		template<typename TSignature>
		bool matchesSignature() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.getEntity(dataIdx).template matchesSignature<TSignature>();
		}

		template<typename T>
		bool hasComponent() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template hasComponent<T>(dataIdx);
		}

		template<typename T>
		auto &getComponent() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template getComponent<T>(dataIdx);
		}

		template<typename T, typename ...Args>
		auto &addComponent(Args&&... args) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template addComponent<T>(dataIdx, std::forward<Args>(args)...);
			return *this;
		}

		template<typename T>
		auto &removeComponent() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template removeComponent<T>(dataIdx);
			return *this;
		}

		template<typename T>
		bool hasTag() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template hasTag<T>(dataIdx);
		}

		template<typename T>
		auto &addTag() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template addTag<T>(dataIdx);
			return *this;
		}

		template<typename T>
		auto &removeTag() const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template removeTag<T>(dataIdx);
			return *this;
		}

		//Overloads meant for type_c tags
		template<typename T>
		auto &removeComponent(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template removeComponent<T>(dataIdx);
			return *this;
		}

		template<typename T, typename ...Args>
		auto &addComponent(el::Type_c<T> const &, Args&&... args) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template addComponent<T>(dataIdx, std::forward<Args>(args)...);
			return *this;
		}

		template<typename T>
		auto &getComponent(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template getComponent<T>(dataIdx);
		}

		template<typename T>
		bool hasComponent(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template hasComponent<T>(dataIdx);
		}

		template<typename T>
		auto &removeTag(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template removeTag<T>(dataIdx);
			return *this;
		}

		template<typename T>
		auto &addTag(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			mgr.template addTag<T>(dataIdx);
			return *this;
		}

		template<typename T>
		bool hasTag(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template hasTag<T>(dataIdx);
		}

		bool operator==(Self const &oth) const noexcept
		{
			return (&this->_mgr == &oth._mgr)
			&& (this->_dataIdx == oth._dataIdx) && (this->_phase == oth._phase);
		}

		template<typename T>
		el::enable_if_t<
			isComponent<T>,
			T &
		> operator[](el::Type_c<T> const &) const noexcept
		{
			return this->template getComponent<T>();
		}

		template<typename T>
		el::enable_if_t<
			isTag<T>,
			bool
		> operator[](el::Type_c<T> const &) const noexcept
		{
			return this->template hasTag<T>();
		}

		template<typename T>
		el::enable_if_t<el::is_type_list<T>::value, bool> operator&(el::Type_c<T> const &) const noexcept
		{
			return Settings::ComponentList::every([this](auto &&comp, auto&&) noexcept {
				return this->template hasComponent<TYPE_OF(comp)>();
			});
		}

		template<typename T>
		el::enable_if_t<isComponent<T>, bool> operator&(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template hasComponent<T>(dataIdx);
		}

		template<typename T>
		el::enable_if_t<isTag<T>, bool> operator&(el::Type_c<T> const &) const noexcept
		{
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(mgr.getHandleData(dataIdx).phase == this->_phase);
			return mgr.template hasTag<T>(dataIdx);
		}
	private:
		Manager &_mgr;
		ecs::HandleDataIdx _dataIdx;
		int _phase;
	};
} // ecs