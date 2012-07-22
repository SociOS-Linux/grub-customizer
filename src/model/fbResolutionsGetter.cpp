#include "fbResolutionsGetter.h"

FbResolutionsGetter::FbResolutionsGetter()
	: eventListener(NULL)
{}

void FbResolutionsGetter::setEventListener(EventListenerView_iface& eventListener){
	this->eventListener = &eventListener;
}

const std::list<std::string>& FbResolutionsGetter::getData() const {
	return data;
}
void FbResolutionsGetter::load(){
	data.clear();
	FILE* hwinfo_proc = popen("hwinfo --framebuffer", "r");
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
	pclose(hwinfo_proc);
	if (this->eventListener)
		this->eventListener->fb_resolutions_loaded();
}
