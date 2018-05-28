/*
 * Entity.h
 *
 *  Created on: 25 sept. 2017
 *      Author: eliord
 */

#ifndef ENTITY_H_
	#define ENTITY_H_
	#include <bitset>

	template<typename TSettings>
	class Entity {
	public:
		using Components = TSettings::Components;
		using Tags = TSettings::Tags;
		Entity();
		virtual ~Entity();

		template<typename TSignature>
		bool matchesSignature(void)
		{
			return TSignature::compare()
		}
	private:

		SignatureBitset	signature;
	};
#endif /* ENTITY_H_ */
