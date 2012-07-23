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

#ifndef SETTING_DLG_GTK_INCLUDED
#define SETTING_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../interface/settingsDlg.h"
#include <libintl.h>
#include "../model/grubEnv.h"
#include "../interface/colorChooser.h"
#include "../interface/evt_settings.h"
#include <string>
#include "../presenter/commonClass.h"


//a gtkmm combobox with colorful foreground and background. useful to choose an item of a predefined color set
class ColorChooserGtk : public Gtk::ComboBox, public ColorChooser {
	struct Columns : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> idName;
		Gtk::TreeModelColumn<Glib::ustring> colorCode_background;
		Gtk::TreeModelColumn<Glib::ustring> colorCode_foreground;
		Columns();
	};
	Columns columns;
	Glib::RefPtr<Gtk::ListStore> refListStore;
	public:
	ColorChooserGtk();
	void addColor(std::string const& codeName, std::string const& outputName, std::string const& cell_background, std::string const& cell_foreground);
	void selectColor(std::string const& codeName);
	std::string getSelectedColor() const;
	Pango::Color getSelectedColorAsPangoObject() const;
};

//a color chooser with predefined colors for grub
class GrubColorChooser : public ColorChooserGtk {
public:
	GrubColorChooser(bool blackIsTransparent = false);
};

class GrubSettingsDlgGtk : public Gtk::Dialog, public SettingsDlg, public CommonClass {
	struct AdvancedSettingsTreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<bool> active;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> old_name;
		Gtk::TreeModelColumn<Glib::ustring> value;
		AdvancedSettingsTreeModel();
	};
	struct CustomOption_obj : public CustomOption {
		CustomOption_obj(std::string name, std::string old_name, std::string value, bool isActive);
	};
	AdvancedSettingsTreeModel asTreeModel;
	Glib::RefPtr<Gtk::ListStore> refAsListStore;
	bool event_lock;
	EventListener_settings* eventListener;
	
	Gtk::Notebook tabbox;
	Gtk::ScrolledWindow scrAllEntries;
	Gtk::TreeView tvAllEntries;
	Gtk::VBox vbAllEntries;
	Gtk::HBox hbAllEntriesControl;
	Gtk::Button bttAddCustomEntry, bttRemoveCustomEntry;

	Gtk::VBox vbCommonSettings, vbAppearanceSettings;
	Gtk::Alignment alignCommonSettings;
	
	Pango::AttrList attrDefaultEntry;
	Pango::Attribute aDefaultEntry;
	//default entry group
	Gtk::Frame groupDefaultEntry;
	Gtk::Alignment alignDefaultEntry;
	Gtk::Label lblDefaultEntry;
	//Gtk::Table tblDefaultEntry;
	Gtk::RadioButton rbDefPredefined, rbDefSaved;
	Gtk::RadioButtonGroup rbgDefEntry;
	Gtk::VBox vbDefaultEntry;
	Gtk::HBox hbDefPredefined;
	//Gtk::SpinButton spDefPosition;
	Gtk::ComboBoxText cbDefEntry;
	std::map<int, std::string> defEntryValueMapping;
	
	//view group
	Gtk::Frame groupView;
	Gtk::Alignment alignView;
	Gtk::Label lblView;
	Gtk::VBox vbView;
	Gtk::CheckButton chkShowMenu, chkOsProber;
	Gtk::HBox hbTimeout;
	Gtk::Label lblTimeout;
	Gtk::SpinButton spTimeout;
	Gtk::Label lblTimeout2;
	
	//kernel parameters
	Gtk::Frame groupKernelParams;
	Gtk::Alignment alignKernelParams;
	Gtk::Label lblKernelParams;
	Gtk::VBox vbKernelParams;
	Gtk::Entry txtKernelParams;
	Gtk::CheckButton chkGenerateRecovery;
	
	//screen resolution
	Gtk::Alignment alignResolution;
	Gtk::HBox hbResolution;
	Gtk::CheckButton chkResolution;
	Gtk::ComboBoxEntryText cbResolution;
	
	//color chooser
	Gtk::Frame groupColorChooser;
	Gtk::Alignment alignColorChooser;
	Gtk::Label lblColorChooser;
	Gtk::Table tblColorChooser;
	Gtk::Label lblforegroundColor, lblBackgroundColor, lblNormalColor, lblHighlightColor;
	GrubColorChooser gccNormalForeground, gccNormalBackground, gccHighlightForeground, gccHighlightBackground;

	//font selection
	Gtk::Frame groupFont;
	Gtk::Label lblFont;
	Gtk::Alignment alignFont;
	Gtk::FontButton bttFont;
	Gtk::HBox hbFont;
	Gtk::Button bttRemoveFont;
	Gtk::Image imgRemoveFont;

	//background image
	Gtk::Frame groupBackgroundImage;
	Gtk::Alignment alignBackgroundImage;
	Gtk::Label lblBackgroundImage, lblBackgroundRequiredInfo;
	Gtk::VBox vbBackgroundImage;
	Gtk::HBox hbBackgroundImage;
	Gtk::FileChooserButton fcBackgroundImage;
	Gtk::DrawingArea drwBackgroundPreview;
	Glib::ustring backgroundImagePath;
	std::list<std::string> previewEntryTitles;
	Glib::Mutex previewEntryTitles_mutex;
	Gtk::HBox hbImgBtts;
	Gtk::VBox vbButtons;
	Gtk::Button bttCopyBackground, bttRemoveBackground;
	Gtk::Image imgRemoveBackground;



	void signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
	void signal_add_row_button_clicked();
	void signal_remove_row_button_clicked();
	void signal_default_entry_predefined_toggeled();
	void signal_default_entry_saved_toggeled();
	void signal_default_entry_changed();
	void signal_showMenu_toggled();
	void signal_osProber_toggled();
	void signal_timeout_changed();
	void signal_kernelparams_changed();
	void signal_recovery_toggled();
	void signal_chkResolution_toggled();
	void signal_resolution_selected();
	void signal_color_changed();
	void signal_font_changed();
	void signal_font_removed();
	void signal_other_image_chosen();
	void signal_bttRemoveBackground_clicked();
	bool signal_redraw_preview(GdkEventExpose* event);
	void on_response(int response_id);
	public:
	GrubSettingsDlgGtk();
	void setEventListener(EventListener_settings& eventListener);
	Gtk::VBox& getCommonSettingsPane();
	Gtk::VBox& getAppearanceSettingsPane();
	void show(bool burgMode);
	void hide();
	ColorChooser& getColorChooser(ColorChooserType type);
	std::string getFontName();
	int getFontSize();
	void setFontName(std::string const& value);
	void addEntryToDefaultEntryChooser(std::string const& labelPathValue, std::string const& labelPathLabel, std::string const& numericPathValue, std::string const& numericPathLabel);
	void clearDefaultEntryChooser();
	void clearResolutionChooser();
	void addResolution(std::string const& resolution);
	std::string getSelectedDefaultGrubValue();
	void addCustomOption(bool isActive, std::string const& name, std::string const& value);
	void selectCustomOption(std::string const& name);
	std::string getSelectedCustomOption();
	void removeAllSettingRows();
	CustomOption getCustomOption(std::string const& name);
	void setActiveDefEntryOption(DefEntryType option);
	DefEntryType getActiveDefEntryOption();
	void setDefEntry(std::string const& defEntry);
	void setShowMenuCheckboxState(bool isActive);
	bool getShowMenuCheckboxState();
	void setOsProberCheckboxState(bool isActive);
	bool getOsProberCheckboxState();
	void showHiddenMenuOsProberConflictMessage();
	void setTimeoutValue(int value);
	int getTimeoutValue();
	std::string getTimeoutValueString();
	void setKernelParams(std::string const& params);
	std::string getKernelParams();
	void setRecoveryCheckboxState(bool isActive);
	bool getRecoveryCheckboxState();
	void setResolutionCheckboxState(bool isActive);
	bool getResolutionCheckboxState();
	void setResolution(std::string const& resolution);
	std::string getResolution();
	Glib::RefPtr<Pango::Layout> createFormattedText(Cairo::RefPtr<Cairo::Context>& context, Glib::ustring const& text, std::string const& format, int r, int g, int b, int r_b, int g_b, int b_b, bool black_bg_is_transparent = true);
	void setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir);
	std::string getBackgroundImagePath();
	void setPreviewEntryTitles(std::list<std::string> const& entries);
};

#endif
