/*
 * Entity.h
 *
 *  Created on: 25 sept. 2017
 *      Author: eliord
 */

#ifndef ENTITY_H_
	#define ENTITY_H_
	#include <bitset>
	#include "Signature.hpp"

	namespace ecs {
		template<typename TSettings>
		class Entity {
		public:
			using Settings = TSettings;
			using Components = typename Settings::Components;
			using Tags = typename Settings::Tags;
			Entity();
			virtual ~Entity();

			template<typename TSignature>
			bool matchesSignature(TSignature&& sig = TSignature())
			{
				return sig.compare(this->_signature);
			}
		private:

			SignatureStorage<Settings> _signature;
		};
	} // ecs
#endif /* ENTITY_H_ */
