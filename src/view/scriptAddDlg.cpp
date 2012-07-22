#include "scriptAddDlg.h"

ScriptAddDlg::ScriptAddDlg()
	: lvScriptPreview(1)
	, lblScriptSelection(gettext("Script to insert:")
	, Gtk::ALIGN_LEFT)
	, lblScriptPreview(gettext("Preview:"), Gtk::ALIGN_LEFT)
{
	this->set_title(gettext("Add script"));
	this->set_icon_name("grub-customizer");
	this->set_default_size(400, 300);
	Gtk::VBox* vbScriptAddDlg = this->get_vbox();
	vbScriptAddDlg->set_spacing(10);
	vbScriptAddDlg->pack_start(hbScriptSelection, Gtk::PACK_SHRINK);
	hbScriptSelection.set_border_width(10);
	hbScriptSelection.pack_start(lblScriptSelection);
	hbScriptSelection.pack_start(cbScriptSelection);
	vbScriptAddDlg->pack_start(vbScriptPreview);
	vbScriptPreview.pack_start(lblScriptPreview, Gtk::PACK_SHRINK);
	vbScriptPreview.pack_start(scrScriptPreview);
	scrScriptPreview.add(lvScriptPreview);
	vbScriptPreview.set_border_width(10);
	scrScriptPreview.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrScriptPreview.set_shadow_type(Gtk::SHADOW_IN);
	lvScriptPreview.set_column_title(0, gettext("Entry"));
	lvScriptPreview.set_headers_visible(false);
	
	this->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	this->add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	this->signal_response().connect(sigc::mem_fun(this, &ScriptAddDlg::signal_scriptAddDlg_response));
	cbScriptSelection.signal_changed().connect(sigc::mem_fun(this, &ScriptAddDlg::signal_script_selection_changed));
}

void ScriptAddDlg::setEventListener(EventListener_scriptAddDlg& eventListener){
	this->eventListener = &eventListener;
}

void ScriptAddDlg::clear(){
	cbScriptSelection.clear_items();
}
void ScriptAddDlg::addItem(Glib::ustring const& text){
	cbScriptSelection.append_text(text);
	cbScriptSelection.set_active(0);
}

void ScriptAddDlg::signal_scriptAddDlg_response(int response_id){
	if (response_id == Gtk::RESPONSE_OK){
		eventListener->scriptAddDlg_applied();
	}
	this->hide();
}

int ScriptAddDlg::getSelectedEntryIndex(){
	return cbScriptSelection.get_active_row_number();
}

void ScriptAddDlg::signal_script_selection_changed(){
	eventListener->scriptSelected();
}

void ScriptAddDlg::clearPreview(){
	lvScriptPreview.clear_items();
}

void ScriptAddDlg::addToPreview(Glib::ustring const& name){
	lvScriptPreview.append_text(name);
	lvScriptPreview.columns_autosize();
}

void ScriptAddDlg::show(){
	this->show_all();
}


