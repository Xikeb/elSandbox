/*
 * Entity.h
 *
 *  Created on: 25 sept. 2017
 *      Author: eliord
 */

#ifndef ENTITY_H_
	#define ENTITY_H_
	#include <cstdlib>
	#include "Signature.hpp"

	namespace ecs {
		using EntityIdx = std::size_t;
		using HandleDataIdx = std::size_t;

		template<typename TSettings>
		class Entity {
		public:
			using Settings = TSettings;

			Entity(ecs::EntityIdx eid, HandleDataIdx hdIndex):
			eid(eid), hdIndex(hdIndex), _alive(true)
			{
				this->_signature.reset();
			}

			Entity(Entity const &oth):
			eid(oth.eid), hdIndex(oth.hdIndex),
			_alive(oth._alive), _signature(oth._signature)
			{
			}

			template<typename TSignature>
			bool matchesSignature(TSignature&& sig = TSignature())
			{
				return sig.compare(this->_signature);
			}

			bool alive() const noexcept
			{
				return this->_alive;
			}

			void kill() noexcept
			{
				this->_alive = false;
			}

			Entity &operator=(Entity const &rhs)
			{
				this->_alive = rhs._alive;
				this->eid = rhs.eid;
				this->hdIndex = rhs.hdIndex;
				this->_signature = rhs._signature;
				return *this;
			}

			template<typename T>
			bool hasComponent() const noexcept
			{
				return this->_signature.template hasComponent<T>();
			}

			template<typename T>
			void addComponent() noexcept
			{
				this->_signature.template enableComponent<T>();
			}

			template<typename T>
			void removeComponent() noexcept
			{
				this->_signature.template disableComponent<T>();
			}

			template<typename T>
			bool hasTag() const noexcept
			{
				return this->_signature.template hasTag<T>();
			}

			template<typename T>
			void addTag() noexcept
			{
				this->_signature.template enableTag<T>();
			}

			template<typename T>
			void removeTag() noexcept
			{
				this->_signature.template disableTag<T>();
			}

			operator bool() const noexcept { return this->alive(); }
			bool operator!() const noexcept { return !this->alive(); }

			ecs::EntityIdx eid;
			ecs::HandleDataIdx hdIndex;

		private:
			bool _alive;
			SignatureBitset<typename Settings::Basic> _signature;
		};
	} // ecs
#endif /* ENTITY_H_ */