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



//a gtkmm combobox with colorful foreground and background. useful to choose an item of a predefined color set
namespace Gc { namespace View { namespace Gtk { namespace Element {
namespace Gtk = ::Gtk;
class ColorChooser :
	public Gtk::ComboBox, public Gc::View::ColorChooser
{
	class Columns : public Gtk::TreeModelColumnRecord {
		public: Gtk::TreeModelColumn<Glib::ustring> name;
		public: Gtk::TreeModelColumn<Glib::ustring> idName;
		public: Gtk::TreeModelColumn<Glib::ustring> colorCode_background;
		public: Gtk::TreeModelColumn<Glib::ustring> colorCode_foreground;
		public: Columns() {
			this->add(this->idName);
			this->add(this->name);
			this->add(this->colorCode_background);
			this->add(this->colorCode_foreground);
		}
	};
	private: Columns columns;
	private: Glib::RefPtr<Gtk::ListStore> refListStore;
	public: bool event_lock = false;

	public:	ColorChooser()
	{
		refListStore = Gtk::ListStore::create(columns);
		this->set_model(refListStore);

		this->pack_start(columns.name);

		Glib::ListHandle<Gtk::CellRenderer*> cellRenderers = this->get_cells();
		Gtk::CellRenderer* cellRenderer = *cellRenderers.begin();

		this->add_attribute(*cellRenderer, "background", columns.colorCode_background);
		this->add_attribute(*cellRenderer, "foreground", columns.colorCode_foreground);
	}

	public:	void addColor(
		std::string const& codeName,
		std::string const& outputName,
		std::string const& cell_background,
		std::string const& cell_foreground
	)
	{
		this->event_lock = true;
		Gtk::TreeModel::iterator iter = refListStore->append();
		(*iter)[columns.idName] = codeName;
		(*iter)[columns.name] = outputName;
		(*iter)[columns.colorCode_background] = cell_background;
		(*iter)[columns.colorCode_foreground] = cell_foreground;
		this->event_lock = false;
	}

	public:	void selectColor(std::string const& codeName)
	{
		this->event_lock = true;
		this->set_active(0);
		for (Gtk::TreeModel::iterator iter = this->get_active(); iter; iter++){
			if ((*iter)[columns.idName] == codeName){
				this->set_active(iter);
				break;
			}
		}
		this->event_lock = false;
	}

	public:	std::string getSelectedColor() const
	{
		Gtk::TreeModel::iterator iter = this->get_active();
		if (iter)
			return (Glib::ustring)(*iter)[columns.idName];
		else
			return "";
	}

	public:	Pango::Color getSelectedColorAsPangoObject() const
	{
		Pango::Color color;
		Gtk::TreeModel::iterator iter = this->get_active();
		if (iter) {
			color.parse((Glib::ustring)(*iter)[columns.colorCode_background]);
		}
		return color;
	}
};}}}}


