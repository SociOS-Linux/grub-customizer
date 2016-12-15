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

#include <string>
#include <list>
#include <functional>


namespace Gc { namespace View { class Theme :
	public Gc::Model::Logger::Trait::LoggerAware
{
	public: enum class EditorType {
		CUSTOM,
		THEME
	};
	public: enum class ColorChooserType {
		DEFAULT_FONT,
		DEFAULT_BACKGROUND,
		HIGHLIGHT_FONT,
		HIGHLIGHT_BACKGROUND
	};

	public: enum class Error {
		INVALID_THEME_PACK_FORMAT,
		RENAME_CONFLICT,
		THEMEFILE_NOT_FOUND,
		SAVE_FAILED,
		NO_FILE_SELECTED
	};

	public: std::function<void (std::string const& name)> onThemeSelected;
	public: std::function<void (std::string const& filePath)> onThemeFileApply;
	public: std::function<void (const std::string& name)> onRemoveThemeClicked;
	public: std::function<void ()> onAddThemeClicked;
	public: std::function<void ()> onSimpleThemeSelected;
	public: std::function<void ()> onAddFile;
	public: std::function<void (std::string const& file)> onRemoveFile;
	public: std::function<void (std::string const& file)> onSelect;
	public: std::function<void (std::string const& newName)> onRename;
	public: std::function<void (std::string const& externalPath)> onFileChoose;
	public: std::function<void (std::string const& newText)> onTextChange;
	public: std::function<void ()> onColorChange;
	public: std::function<void (bool removeFont)> onFontChange;
	public: std::function<void ()> onImageChange;
	public: std::function<void ()> onImageRemove;
	public: std::function<void ()> onSaveClick;

	public: virtual void addFile(std::string const& fileName) = 0;
	public: virtual void clear() = 0;
	public: virtual void setText(std::string const& text) = 0;
	public: virtual void setImage(std::string const& path) = 0;
	public: virtual void selectFile(std::string const& fileName, bool startEdit = false) = 0;
	public: virtual void selectTheme(std::string const& name) = 0;
	public: virtual std::string getSelectedTheme() = 0;
	public: virtual void addTheme(std::string const& name) = 0;
	public: virtual void clearThemeSelection() = 0;
	public: virtual void show(bool burgMode) = 0;
	public: virtual void setRemoveFunctionalityEnabled(bool value) = 0;
	public: virtual void setEditorType(EditorType type) = 0;
	public: virtual void showThemeFileChooser() = 0;
	public: virtual void showError(Error const& e, std::string const& info = "") = 0;
	public: virtual void setCurrentExternalThemeFilePath(std::string const& fileName) = 0;
	public: virtual std::string getDefaultName() const = 0;

	//returns an interface to the given color chooser
	public: virtual Gc::View::ColorChooser& getColorChooser(ColorChooserType type) = 0;
	//get the name of the selected font
	public: virtual std::string getFontName() = 0;
	//get the font size
	public: virtual int getFontSize() = 0;
	//set the name of the selected font
	public: virtual void setFontName(std::string const& value) = 0;

	//shows a warning message about very large fonts
	public: virtual void showFontWarning() = 0;

	//sets the background image, remove it, if empty string is given
	public: virtual void setBackgroundImagePreviewPath(std::string const& menuPicturePath, bool isInGrubDir) = 0;
	//reads the selected background image path
	public: virtual std::string getBackgroundImagePath() = 0;

	//set the entry titles to be viewed in the preview image
	public: virtual void setPreviewEntryTitles(std::list<std::string> const& entries) = 0;

	public: virtual inline ~Theme() {};
};}}

