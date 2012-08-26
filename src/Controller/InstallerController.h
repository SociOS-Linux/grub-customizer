/*
 * InstallerController.h
 *
 *  Created on: 26.08.2012
 *      Author: root
 */

#ifndef INSTALLERCONTROLLER_H_
#define INSTALLERCONTROLLER_H_

#include "ThreadController.h"

class InstallerController {
public:
	virtual inline ~InstallerController(){};
	virtual void showAction() = 0;
	virtual void installGrubAction(std::string device) = 0;
	virtual void installGrubThreadedAction(std::string device) = 0;
	virtual void showMessageAction(std::string const& msg) = 0;
	virtual ThreadController& getThreadController() = 0;
};


#endif /* INSTALLERCONTROLLER_H_ */
