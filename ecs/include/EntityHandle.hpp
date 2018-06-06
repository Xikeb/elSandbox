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

			EntityHandle(ecs::Manager<Settings> &mgr, ecs::HandleDataIdx dataIdx, int phase):
			_mgr(mgr), _dataIdx(dataIdx), _phase(phase)
			{
			}

			int getPhase() const noexcept
			{
				return this->_phase;
			}

			auto &getManager() noexcept
			{
				return this->_mgr;
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
				return mgr.getComponent<T>(dataIdx);
			}

			template<typename T, typename ...Args>
			auto &addComponent(Args&&... args) noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				return mgr.template addComponent<T>(dataIdx, std::forward<Args>(args)...);
			}

			template<typename T>
			auto removeComponent() const noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				mgr.removeComponent<T>(dataIdx);
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
			void addTag() noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				mgr.template addTag<T>(dataIdx);
			}

			template<typename T>
			void removeTag() noexcept
			{
				auto &mgr = this->_mgr;
				auto dataIdx = this->_dataIdx;

				assert(mgr.getHandleData(dataIdx).phase == this->_phase);
				mgr.template removeTag<T>(dataIdx);
			}

		private:
			ecs::Manager<Settings> &_mgr;
			ecs::HandleDataIdx _dataIdx;
			int _phase;
		};
	} // ecs
#endif /* ENTITYHANDLE_H_ */
