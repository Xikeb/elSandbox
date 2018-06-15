/*
 * EntityHandle.h
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#ifndef ENTITYHANDLE_H_
	#define ENTITYHANDLE_H_
	#include <cassert>
	#include "Entity.hpp"

	namespace ecs {template<typename TSettings>
		class Manager;

		template<typename TSettings>
		class EntityHandle {
		public:
			using Settings = TSettings;
			using This = ecs::EntityHandle<TSettings>;

			explicit EntityHandle(ecs::Manager<Settings> &mgr) noexcept:
			_mgr(mgr), _dataIdx(-1), _phase(-1)
			{
			}

			EntityHandle(
				ecs::Manager<Settings> &mgr,
				ecs::HandleDataIdx dataIdx,
				int phase
			): _mgr(mgr), _dataIdx(dataIdx), _phase(phase)
			{
			}

			EntityHandle(This &oth):
			_mgr(oth._mgr), _dataIdx(oth._dataIdx),
			_phase(oth._phase)
			{
			}

			EntityHandle(This &&oth):
			_mgr(oth._mgr), _dataIdx(oth._dataIdx),
			_phase(oth._phase)
			{
			}

			This &operator=(This const &oth) noexcept
			{
				return *(new (this) This(oth));
			}

			This &operator=(This &&oth) noexcept
			{
				return *(new (this) This(oth));
			}

			int getPhase() const noexcept
			{
				return this->_phase;
			}

			auto &getManager() noexcept
			{
				return this->_mgr;
			}

			bool isValid() const noexcept
			{
				return (this->_mgr.getHandleData\
					(this->_dataIdx).phase == this->_phase);
			}

			void kill() noexcept
			{
				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				this->_mgr.getEntity(this->_dataIdx).kill();
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
			bool hasComponent() noexcept
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
			auto &addComponent(Args&&... args) noexcept
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
			bool hasTag() noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				return mgr.template hasTag<T>(dataIdx);
			}

			template<typename T>
			auto &addTag() noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				mgr.template addTag<T>(dataIdx);
				return *this;
			}

			template<typename T>
			auto &removeTag() noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				mgr.template removeTag<T>(dataIdx);
				return *this;
			}

			bool operator==(This const &oth)
			{
				return (&this->_mgr == &oth._mgr)
				&& (this->_dataIdx == oth._dataIdx) && (this->_phase == oth._phase);
			}

		private:
			ecs::Manager<Settings> &_mgr;
			ecs::HandleDataIdx _dataIdx;
			int _phase;
		};
	} // ecs
#endif /* ENTITYHANDLE_H_ */
