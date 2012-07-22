#include "aboutDialogGtk.h"

void AboutDialogGtk::signal_about_dlg_response(int response_id){
	if (Gtk::RESPONSE_CLOSE)
		this->hide();
}

AboutDialogGtk::AboutDialogGtk()
	: appName("Grub Customizer"), appVersion(GC_VERSION)
{
	this->set_name(appName);
	this->set_version(appVersion);
	this->authors.push_back("Daniel Richter https://launchpad.net/~danielrichter2007");
	this->set_authors(authors);

	this->set_icon_name("grub-customizer");
	this->set_logo_icon_name("grub-customizer");
	this->set_comments(gettext("Grub Customizer is a graphical interface to configure the grub2/burg settings"));

	this->set_translator_credits(
"Adam Czabara https://launchpad.net/~adam-czabara\n\
Alexey Ivanov https://launchpad.net/~alexey.ivanes\n\
Bernardo Miguel Savone https://launchpad.net/~bersil\n\
Careone https://launchpad.net/~zzbusagain\n\
Daniel Richter https://launchpad.net/~danielrichter2007\n\
David Jones https://launchpad.net/~bikerboi87\n\
Emre AYTAÇ https://launchpad.net/~eaytac\n\
Erkin Batu Altunbaş https://launchpad.net/~erkin\n\
Eugênio F https://launchpad.net/~eugf\n\
Fedik https://launchpad.net/~fedikw\n\
GamePad64 https://launchpad.net/~gamepad64\n\
gilles chaon https://launchpad.net/~g-chaon\n\
GoJoMo https://launchpad.net/~tolbkni\n\
Jan Bažant aka Brbla https://launchpad.net/~brbla\n\
Jarosław Ogrodnik https://launchpad.net/~goz\n\
Jörg BUCHMANN https://launchpad.net/~jorg-buchmann\n\
José Humberto Melo https://launchpad.net/~josehumberto-melo\n\
Manuel Xosé Lemos https://launchpad.net/~mxlemos\n\
Maxime Gentils https://launchpad.net/~maxime.gentils\n\
Michael Kotsarinis https://launchpad.net/~mk73628\n\
Miguel Anxo Bouzada https://launchpad.net/~mbouzada\n\
nafterburner https://launchpad.net/~nafterburner\n\
Octaiver Matt https://launchpad.net/~octaivermatt\n\
patel https://launchpad.net/~patel\n\
shishimaru https://launchpad.net/~salvi-uchiha\n\
Svetoslav Stefanov https://launchpad.net/~svetlisashkov\n\
TheMengzor https://launchpad.net/~the-mengzor\n\
Thibaut Panis https://launchpad.net/~thibaut-panis\n\
zeugma https://launchpad.net/~sunder67\
");
	this->signal_response().connect(sigc::mem_fun(this, &AboutDialogGtk::signal_about_dlg_response));
}

void AboutDialogGtk::show(){
	Gtk::AboutDialog::show();
}
