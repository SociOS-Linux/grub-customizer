#ifndef COLOR_CHOOSER_INCLUDED
#define COLOR_CHOOSER_INCLUDED
#include <string>
class ColorChooser {
public:
	virtual void selectColor(std::string const& codeName)=0;
	virtual std::string getSelectedColor() const=0;
};

#endif
