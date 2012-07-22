#ifndef SETTING_DLG_GTK_INCLUDED
#define SETTING_DLG_GTK_INCLUDED
#include <gtkmm.h>
#include "../model/settings_mng_ds.h"
#include "../model/fbResolutionsGetter.h"
#include "../interface/settings_dlg.h"
#include <libintl.h>
#include "../model/grubEnv.h"
#include "../interface/colorChooser.h"
#include "../interface/eventListener_view_iface.h"
#include <string>


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
};

//a color chooser with predifined colors for grub
class GrubColorChooser : public ColorChooserGtk {
public:
	GrubColorChooser(bool blackIsTransparent = false);
};


class GrubSettingsDlgGtk : public Gtk::Dialog, public GrubSettingsDlg {
	struct AdvancedSettingsTreeModel : public Gtk::TreeModelColumnRecord {
		Gtk::TreeModelColumn<bool> active;
		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> value;
		AdvancedSettingsTreeModel();
	};
	AdvancedSettingsTreeModel asTreeModel;
	Glib::RefPtr<Gtk::ListStore> refAsListStore;
	GrubEnv& env;
	bool event_lock;
	EventListenerView_iface* eventListener;
	
	SettingsManagerDataStore* dataStore;
	Gtk::Notebook tabbox;
	Gtk::ScrolledWindow scrAllEntries;
	Gtk::TreeView tvAllEntries;
	Gtk::VBox vbCommonSettings, vbAppearanceSettings;
	
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
	int defEntry_item_count;
	
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
	
	//background image
	Gtk::Frame groupBackgroundImage;
	Gtk::Alignment alignBackgroundImage;
	Gtk::Label lblBackgroundImage, lblBackgroundRequiredInfo;
	Gtk::VBox vbBackgroundImage;
	Gtk::FileChooserButton fcBackgroundImage;
	Gtk::Image imgBackgroundImage;
	Gtk::HBox hbImgBtts;
	Gtk::VBox vbButtons;
	Gtk::Button bttCopyBackground, bttRemoveBackground;
	
	void signal_setting_row_changed(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
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
	void signal_other_image_chosen();
	void signal_bttCopyBackground_clicked();
	void signal_bttRemoveBackground_clicked();
	public:
	enum DefEntryType {
		DEF_ENTRY_PREDEFINED,
		DEF_ENTRY_SAVED
	};
	enum ColorChooserType {
		COLOR_CHOOSER_DEFAULT_FONT,
		COLOR_CHOOSER_DEFAULT_BACKGROUND,
		COLOR_CHOOSER_HIGHLIGHT_FONT,
		COLOR_CHOOSER_HIGHLIGHT_BACKGROUND
	};
	struct CustomOption {
		std::string name, value;
		bool isActive;
		CustomOption(std::string name, std::string value, bool isActive);
	};
	GrubSettingsDlgGtk(SettingsManagerDataStore& dataStore, GrubEnv& env);
	void setEventListener(EventListenerView_iface& eventListener);
	void show();
	ColorChooser& getColorChooser(ColorChooserType type);
	void addEntryToDefaultEntryChooser(std::string const& entryTitle);
	void clearDefaultEntryChooser();
	void clearResolutionChooser();
	void addResolution(std::string const& resolution);
	void syncSettings();
	Glib::ustring getSelectedDefaultGrubValue();
	void updateDefaultSetting();
	void updateCustomSetting(std::string const& name);
	void addCustomOption(bool isActive, Glib::ustring const& name, Glib::ustring const& value);
	void removeAllSettingRows();
	CustomOption getCustomOption(Glib::ustring const& name);
	void setActiveDefEntryOption(DefEntryType option);
	DefEntryType getActiveDefEntryOption();
	void setDefEntry(Glib::ustring const& defEntry);
	void setShowMenuCheckboxState(bool isActive);
	bool getShowMenuCheckboxState();
	void setOsProberCheckboxState(bool isActive);
	bool getOsProberCheckboxState();
	void showHiddenMenuOsProberConflictMessage();
	void updateShowMenuSetting();
	void updateOsProberSetting();
	void setTimeoutValue(int value);
	int getTimeoutValue();
	void updateTimeoutSetting();
	void setKernelParams(Glib::ustring const& params);
	Glib::ustring getKernelParams();
	void updateKernalParams();
	void setRecoveryCheckboxState(bool isActive);
	bool getRecoveryCheckboxState();
	void updateGenerateRecoverySetting();
	void setResolutionCheckboxState(bool isActive);
	bool getResolutionCheckboxState();
	void updateUseCustomResolution();
	void setResolution(Glib::ustring const& resolution);
	Glib::ustring getResolution();
	void updateCustomResolution();
	void setBackgroundImagePreviewPath(Glib::ustring const& menuPicturePath, bool isInGrubDir);
	void updateColorSettings();
	Glib::ustring getBackgroundImagePath();
	void updateBackgroundImage();
	void copyBackgroundImageToGrubDirectory();
	void removeBackgroundImage();
};

#endif
