#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

/**
 * base class to be implemented by about dialogs
 */
class AboutDialog {
public:
	//show the about dialog, don't block
	virtual void show()=0;
};

#endif
