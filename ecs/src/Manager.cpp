/*
 * Manager.cpp
 *
 *  Created on: 23 oct. 2017
 *      Author: eliord
 */

#include "Manager.hpp"

Manager::Manager()
{
	// TODO Auto-generated constructor stub

}

Manager::~Manager()
{
	// TODO Auto-generated destructor stub
}

Manager::enlarge(std::size_t addition)
{
	addition = (1 + addition % 128) * 128;
	for (auto &&sto: this->_componentStorage)
		sto.reserve(this->_capacity + addition);
	this->_capacity += addition;
}