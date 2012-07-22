#ifndef COLOR_CHOOSER_INCLUDED
#define COLOR_CHOOSER_INCLUDED
#include <string>
/**
 * base class to be implemented by color chooser controls
 */
class ColorChooser {
public:
	//choose one of the colors provided by this control
	virtual void selectColor(std::string const& codeName)=0;

	//determine which color is currently selected
	virtual std::string getSelectedColor() const=0;
};

#endif
