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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef SRC_VIEW_GTK_ELEMENT_GRUBCOLORCHOOSER_HPP_
#define SRC_VIEW_GTK_ELEMENT_GRUBCOLORCHOOSER_HPP_


//a color chooser with predefined colors for grub
namespace Gc { namespace View { namespace Gtk { namespace Element { class GrubColorChooser :
	public Gc::View::Gtk::Element::ColorChooser
{
	public: GrubColorChooser(bool blackIsTransparent = false) :
		Gc::View::Gtk::Element::ColorChooser()
	{
		this->addColor("white",          gettext("white"),         "#ffffff", "#000000");
		this->addColor("yellow",         gettext("yellow"),        "#fefe54", "#000000");
		this->addColor("light-cyan",     gettext("light-cyan"),    "#54fefe", "#000000");
		this->addColor("cyan",           gettext("cyan"),          "#00a8a8", "#000000");
		this->addColor("light-blue",     gettext("light-blue"),    "#5454fe", "#000000");
		this->addColor("blue",           gettext("blue"),          "#0000a8", "#000000");
		this->addColor("light-green",    gettext("light-green"),   "#54fe54", "#000000");
		this->addColor("green",          gettext("green"),         "#00a800", "#000000");
		this->addColor("light-magenta",  gettext("light-magenta"), "#eb4eeb", "#000000");
		this->addColor("magenta",        gettext("magenta"),       "#a800a8", "#000000");
		this->addColor("light-red",      gettext("light-red"),     "#fe5454", "#000000");
		this->addColor("red",            gettext("red"),           "#ff0000", "#000000");
		this->addColor("brown",          gettext("brown"),         "#a85400", "#000000");
		this->addColor("light-gray",     gettext("light-gray"),    "#a8a8a8", "#000000");
		this->addColor("dark-gray",      gettext("dark-gray"),     "#545454", "#000000");
		this->addColor("black", blackIsTransparent ? gettext("transparent") : gettext("black"), "#000000", "#ffffff");
	}

};}}}}


#endif /* SRC_VIEW_GTK_ELEMENT_GRUBCOLORCHOOSER_HPP_ */
