/*
 * EntryNameImpl.h
 *
 *  Created on: 25.08.2012
 *      Author: root
 */

#ifndef ENTRYNAME_H_
#define ENTRYNAME_H_

#include <string>
#include "../Model/Entry.h"

class Mapper_EntryName {
public:
	virtual inline ~Mapper_EntryName(){};
	virtual std::string map(Model_Entry const* sourceEntry, std::string const& defaultName, std::string const& scriptName = "") = 0;
};


#endif /* ENTRYNAME_H_ */
