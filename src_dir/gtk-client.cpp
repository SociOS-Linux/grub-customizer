#include "grubconfig.h"
#include "grubconf_ui_gtk.h"
#include <libintl.h>
#include <locale.h>

int main(int argc, char** argv){
	setlocale( LC_ALL, "");
	bindtextdomain( "default", "/opt/grub-customizer/locale");
	textdomain( "default" );

	std::cout << gettext("This is Argument 0: ") << argv[0] << std::endl;

	Gtk::Main app(argc, argv);
	Glib::thread_init();

	GrubConfig config;
	GrubConfUIGtk ui(config);
	config.connectUI(ui);
	
	//config.load();
	
	/*for (std::map<std::string, EntryList>::iterator iter = config.realScripts.begin(); iter != config.realScripts.end(); iter++){
		std::cout << iter->first << std::endl;
		for (EntryList::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++){
			std::cout << "\t" << iter2->name << std::endl;
		}
	}*/
	//	config.realScripts[scriptName].entries.push_back(grubEntry);
	
	ui.run();
	
	//show config
	
	/*for (std::list<ToplevelScript>::iterator iter = config.begin(); iter != config.end(); iter++){
		std::cout << iter->name << (!iter->isExecutable() ? " (deaktiviert)" : "")
		<< (iter->proxyfiedScriptName != "" ? " ("+iter->proxyfiedScriptName+")" : "")
		<< std::endl;
		for (std::list<Entry>::iterator entryIter = iter->entries.begin(); entryIter != iter->entries.end(); entryIter++){
			std::cout << "           " << (entryIter->disabled ? "[" : "") << entryIter->name << (entryIter->disabled ? "]" : "")
			<< (entryIter->outputName != entryIter->name ? " \""+entryIter->outputName+"\"" : "") << std::endl;
		}
	}*/
}







