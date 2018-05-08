/*
 * EntityHandle.h
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#ifndef ENTITYHANDLE_H_
#define ENTITYHANDLE_H_

class EntityHandle
{
public:
	EntityHandle();
	virtual ~EntityHandle();
	int		getPhase();
	EntityHandle	phaseUp();

private:
	int		phase;
	Entity	e;
};

#endif /* ENTITYHANDLE_H_ */
