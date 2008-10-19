#include <iostream>
#include <gtk/gtk.h>

#include "pathsupport.h"
#include "fileexteditor.h"
#include "prefsdialog.h"

#include "config.h"
#include "gettext.h"
#define _(x) gettext(x)
#define N_(x) gettext_noop(x)

using namespace std;

#define RESPONSE_SAVE 1

bool PrefsDialog(MainWindow *mw)
{
	GtkWidget *window=gtk_dialog_new_with_buttons(_("Settings..."),
		GTK_WINDOW(mw->GetWindow()),GtkDialogFlags(0),
		GTK_STOCK_SAVE,RESPONSE_SAVE,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		NULL);
	
	GtkWidget *vbox=gtk_vbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox),vbox,TRUE,TRUE,8);
	
	GtkWidget *hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	GtkWidget *tmp=gtk_label_new(_("Copy photos to folders within: "));
	gtk_box_pack_start(GTK_BOX(hbox),tmp,FALSE,FALSE,5);
	GtkWidget *dirsel=gtk_file_chooser_button_new (_("Copy files to: "),GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_box_pack_start(GTK_BOX(hbox),dirsel,TRUE,TRUE,5);

	char *dp=substitute_homedir(mw->sessionlist->FindString("DestPath"));	
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dirsel),dp);
	free(dp);

	GtkWidget *label=gtk_label_new(_("Exclude folders named:"));
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);

	GtkWidget *excleditor=fileexteditor_new();
	gtk_box_pack_start(GTK_BOX(vbox),excleditor,TRUE,TRUE,0);

	fileexteditor_setextstring(FILEEXTEDITOR(excleditor),mw->sessionlist->FindString("Exclusions"));


	tmp=gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),tmp,FALSE,FALSE,5);


	label=gtk_label_new(_("Copy files of type:"));
	gtk_box_pack_start(GTK_BOX(vbox),label,false,false,0);

	GtkWidget *exteditor=fileexteditor_new(_("File Extension"));
	gtk_box_pack_start(GTK_BOX(vbox),exteditor,TRUE,TRUE,0);

	fileexteditor_setextstring(FILEEXTEDITOR(exteditor),mw->sessionlist->FindString("Extensions"));

	gtk_widget_show_all (window);

	bool done=false;
	bool save=false;
	bool result=false;
	while(!done)
	{
		gint result=gtk_dialog_run(GTK_DIALOG(window));
		switch(result)
		{
			case GTK_RESPONSE_CANCEL:
				done=true;
				break;
			case RESPONSE_SAVE:
				{
					save=true;
					// Fall through to OK
				}
			case GTK_RESPONSE_OK:
				{
					char *dirname = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dirsel));
					mw->sessionlist->SetString("DestPath",dirname);
					char *extlist=fileexteditor_getextstring(FILEEXTEDITOR(exteditor));
					mw->sessionlist->SetString("Extensions",extlist);
					free(extlist);
					char *exclist=fileexteditor_getextstring(FILEEXTEDITOR(excleditor));
					mw->sessionlist->SetString("Exclusions",exclist);
					free(exclist);
					mw->sessionlist->SetExtensions();
					mw->sessionlist->SetExclusions();
				}
				done=true;
				result=true;
				break;
		}
	}
	if(save)
	{
		mw->SaveSettings();
		// Save new options
	}

	gtk_widget_destroy(window);
	
	return(result);
}
