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

		template<typename T, typename U>
		using ifIsComponent = el::enable_if_t<isComponent<T>, U>;
		template<typename T, typename U>
		using ifIsTag = el::enable_if_t<isTag<T>, U>;

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

		//Copy-assignment
		Self &operator=(Self const &oth) noexcept {
			//Placement-new is required because of reference to manager
			return *(new (this) Self(oth));
		}

		int getPhase() const noexcept { return this->_phase; }
		auto &getManager() noexcept { return this->_mgr; }

		//Whether this handle has the same phase as the entity it represents (handle is invalid otherwise)
		bool isInPhase() const noexcept {
			return (this->_mgr.getHandleData(this->_dataIdx).phase == this->_phase);
		}

		// What purpose does it have??
		bool isValid() const noexcept {
			return this->isInPhase() && this->_mgr.getEntity(this->_dataIdx);
		}

		void kill() noexcept {
			// auto &mgr = this->_mgr;
			// auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			this->_mgr.killEntity(this->_dataIdx);
		}

		template<typename TSignature>
		bool matchesSignature(TSignature &&sig) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.getEntity(dataIdx).matchesSignature(std::forward<TSignature>(sig));
		}

		template<typename TSignature>
		bool matchesSignature() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.getEntity(dataIdx).template matchesSignature<TSignature>();
		}

		/*
		 * Check for presence of component
		*/
		template<typename T>
		bool hasComponent() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template hasComponent<T>(dataIdx);
		}

		template<typename T>
		bool hasComponent(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template hasComponent<T>(dataIdx);
		}

		/*
		 * Check for presence of component
		*/
		template<typename T>
		auto &getComponent() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template getComponent<T>(dataIdx);
		}

		template<typename T>
		auto &getComponent(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template getComponent<T>(dataIdx);
		}

		template<typename T, typename ...Args>
		auto &addComponent(Args&&... args) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addComponent<T>(dataIdx, std::forward<Args>(args)...);
			return *this;
		}

		template<typename T, typename ...Args>
		auto &addComponent(el::Type_c<T> const &, Args&&... args) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addComponent<T>(dataIdx, std::forward<Args>(args)...);
			return *this;
		}

		template<typename T>
		auto &removeComponent() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template removeComponent<T>(dataIdx);
			return *this;
		}

		template<typename T>
		auto &removeComponent(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template removeComponent<T>(dataIdx);
			return *this;
		}

		template<typename T>
		bool hasTag() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template hasTag<T>(dataIdx);
		}

		template<typename T>
		bool hasTag(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template hasTag<T>(dataIdx);
		}

		template<typename T>
		auto &addTag() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addTag<T>(dataIdx);
			return *this;
		}

		template<typename T>
		auto &addTag(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addTag<T>(dataIdx);
			return *this;
		}

		template<typename T>
		auto &removeTag() const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template removeTag<T>(dataIdx);
			return *this;
		}

		template<typename T>
		auto &removeTag(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template removeTag<T>(dataIdx);
			return *this;
		}

		bool operator==(Self const &oth) const noexcept {
			return (&this->_mgr == &oth._mgr)
				&& (this->_dataIdx == oth._dataIdx)
				&& (this->_phase == oth._phase);
		}

		template<typename T>
		ifIsComponent<T, T&> operator[](el::Type_c<T> const &) const noexcept {
			return this->template getComponent<T>();
		}

		template<typename T>
		ifIsTag<T, bool> operator[](el::Type_c<T> const &) const noexcept {
			return this->template hasTag<T>();
		}

		template<typename Types>
		el::enable_if_t<el::is_type_list<Types>::value, bool> operator&(el::Type_c<Types> const &) const noexcept {
			return Types::every([this](auto &&elem, auto&&) noexcept {
				return *this & elem;
			});
		}

		template<typename T>
		ifIsComponent<T, bool> operator&(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template hasComponent<T>(dataIdx);
		}

		template<typename T>
		ifIsTag<T, bool> operator&(el::Type_c<T> const &) const noexcept {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			return mgr.template hasTag<T>(dataIdx);
		}

        template<typename T>
		ifIsComponent<T, Self&> operator<<(el::Type_c<T> const &) {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addComponent<T>(dataIdx);
			return *this;
		}

        template<typename T>
		ifIsTag<T, Self&> operator<<(el::Type_c<T> const &) {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addTag<T>(dataIdx);
			return *this;
		}

        template<typename T>
		ifIsComponent<T, Self&> operator<<(T &&t) {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template addComponent<T>(dataIdx, std::forward<T>(t));
			return *this;
		}

        template<typename T>
		ifIsComponent<T, Self&> operator>>(el::Type_c<T> const &) {
			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			mgr.template removeComponent<T>(dataIdx);
			return *this;
		}

        template<typename T>
		ifIsComponent<T, Self&> operator>>(T &t) {

			auto &mgr = this->_mgr;
			auto dataIdx = this->_dataIdx;

			assert(this->isInPhase());
			t = mgr.template getComponent<T>(dataIdx);
			return *this;
		}

		//Add a toggle component/tag operator

	private:
		Manager &_mgr;
		ecs::HandleDataIdx _dataIdx;
		int _phase;
	};
} // ecs
