/*
 * Copyright (C) 2010-2011 Daniel Richter <danielrichter2007@web.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "About.h"

void View_Gtk_About::signal_about_dlg_response(int response_id){
	if (Gtk::RESPONSE_CLOSE)
		this->hide();
}

View_Gtk_About::View_Gtk_About()
	: appName("Grub Customizer"), appVersion(GC_VERSION)
{
	this->set_name(appName);
	this->set_version(appVersion);
	this->authors.push_back("Daniel Richter https://launchpad.net/~danielrichter2007");
	this->set_authors(authors);

	this->set_icon_name("grub-customizer");
	this->set_logo_icon_name("grub-customizer");
	this->set_comments(gettext("Grub Customizer is a graphical interface to configure the grub2/burg settings"));

	this->artists.push_back("Zelozelos");
	this->set_artists(artists);

	this->set_translator_credits(
"Adam Czabara https://launchpad.net/~adam-czabara\n\
Alexey Ivanov https://launchpad.net/~alexey.ivanes\n\
ASTUR2000 https://launchpad.net/~enolsp94\n\
Badver https://launchpad.net/~badver\n\
Bernardo Miguel Savone https://launchpad.net/~bersil\n\
Careone https://launchpad.net/~zzbusagain\n\
DAG Software https://launchpad.net/~dagsoftware\n\
Daniel Richter https://launchpad.net/~danielrichter2007\n\
David Jones https://launchpad.net/~bikerboi87\n\
Dimitar Dimitrov https://launchpad.net/~dimitrov\n\
Draky https://launchpad.net/~draky\n\
Emre AYTAÇ https://launchpad.net/~eaytac\n\
Erkin Batu Altunbaş https://launchpad.net/~erkin\n\
Eugene Marshal https://launchpad.net/~lowrider\n\
Eugênio F https://launchpad.net/~eugf\n\
Fedik https://launchpad.net/~fedikw\n\
GamePad64 https://launchpad.net/~gamepad64\n\
gilles chaon https://launchpad.net/~g-chaon\n\
GoJoMo https://launchpad.net/~tolbkni\n\
harvalikjan@gmail.com https://launchpad.net/~harvalikjan\n\
HSFF https://launchpad.net/~hsff\n\
I. De Marchi https://launchpad.net/~tangram-peces\n\
Jan Bažant aka Brbla https://launchpad.net/~brbla\n\
Jarosław Ogrodnik https://launchpad.net/~goz\n\
jmb_kz https://launchpad.net/~jmb-kz\n\
Jörg BUCHMANN https://launchpad.net/~jorg-buchmann\n\
José Humberto Melo https://launchpad.net/~josehumberto-melo\n\
Lê Trường An. https://launchpad.net/~truongan\n\
Mantas Kriaučiūnas https://launchpad.net/~mantas\n\
Manuel Xosé Lemos https://launchpad.net/~mxlemos\n\
Maks Lyashuk aka Ma)(imuM https://launchpad.net/~probel\n\
Maxime Gentils https://launchpad.net/~maxime.gentils\n\
Michael Kotsarinis https://launchpad.net/~mk73628\n\
Miguel Anxo Bouzada https://launchpad.net/~mbouzada\n\
nafterburner https://launchpad.net/~nafterburner\n\
Octaiver Matt https://launchpad.net/~octaivermatt\n\
patel https://launchpad.net/~patel\n\
rob https://launchpad.net/~rvdb\n\
shishimaru https://launchpad.net/~salvi-uchiha\n\
Svetoslav Stefanov https://launchpad.net/~svetlisashkov\n\
TheMengzor https://launchpad.net/~the-mengzor\n\
Thibaut Panis https://launchpad.net/~thibaut-panis\n\
Tielhalter https://launchpad.net/~tielhalter\n\
Timo Seppola https://launchpad.net/~timo-seppola\n\
Tomas Hasko https://launchpad.net/~thasko\n\
Tubuntu https://launchpad.net/~t-ubuntu\n\
Zbyněk Schwarz https://launchpad.net/~tsbook\n\
zeugma https://launchpad.net/~sunder67\
");
	this->signal_response().connect(sigc::mem_fun(this, &View_Gtk_About::signal_about_dlg_response));
}

void View_Gtk_About::show(){
	Gtk::AboutDialog::show();
}
