#include "fbResolutionsGetter.h"

FbResolutionsGetter::FbResolutionsGetter()
	: eventListener(NULL), _isLoading(false)
{}

void FbResolutionsGetter::setEventListener(EventListener_model& eventListener){
	this->eventListener = &eventListener;
}

const std::list<std::string>& FbResolutionsGetter::getData() const {
	return data;
}
void FbResolutionsGetter::load(){
	if (!_isLoading){ //make sure that only one thread is running this function at the same time
		_isLoading = true;
		data.clear();
		FILE* hwinfo_proc = popen("hwinfo --framebuffer", "r");
		if (hwinfo_proc){
			int c;
			std::string row;
			//parses mode lines like "  Mode 0x0300: 640x400 (+640), 8 bits"
			while ((c = fgetc(hwinfo_proc)) != EOF){
				if (c != '\n')
					row += char(c);
				else {
					if (row.substr(0,7) == "  Mode "){
						int beginOfResulution = row.find(':')+2;
						int endOfResulution = row.find(' ', beginOfResulution);

						int beginOfColorDepth = row.find(' ', endOfResulution+1)+1;
						int endOfColorDepth = row.find(' ', beginOfColorDepth);

						data.push_back(
							row.substr(beginOfResulution, endOfResulution-beginOfResulution)
						  + "x"
						  + row.substr(beginOfColorDepth, endOfColorDepth-beginOfColorDepth)
						);
					}
					row = "";
				}
			}
			if (pclose(hwinfo_proc) == 0 && this->eventListener)
				this->eventListener->fb_resolutions_loaded();
		}
		_isLoading = false;
	}
}
