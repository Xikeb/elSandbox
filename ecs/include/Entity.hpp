/*
 * Entity.h
 *
 *  Created on: 25 sept. 2017
 *      Author: eliord
 */

#ifndef ENTITY_H_
	#define ENTITY_H_

	class Entity {
	public:
		Entity();
		virtual ~Entity();
		bool	hasComponent<class C>();
		bool	attachComponent<class C>(Manager world, EntityHandle h);
		bool	detachComponen<class C>(Manager world, EntityHandle h);
		bool	inUse();
		bool	inUse(bool state);

	private:
		bool			inUse;
		size_t			componentColumn;
		SignatureBitset	signature;
	};
#endif /* ENTITY_H_ */
