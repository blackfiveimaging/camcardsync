#include <iostream>

#include <gtk/gtk.h>

#include "config.h"
#include "camsession.h"
#include "progressbar.h"
#include "pathsupport.h"
#include "prefsdialog.h"

#include "mainwindow.h"

#include "config.h"
#include "gettext.h"
#define _(x) gettext(x)
#define N_(x) gettext_noop(x)

using namespace std;

/* columns */
enum
{
  ACTIVE_COLUMN = 0,
  LABEL_COLUMN,
  INFO_COLUMN,
  TRUE_COLUMN,
  CAMSESSION_COLUMN,
  NUM_COLUMNS
};


static GtkTreeModel *create_model ()
{
	GtkTreeStore *model;

	/* create tree store */
	model = gtk_tree_store_new (NUM_COLUMNS,
		G_TYPE_BOOLEAN,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_BOOLEAN,
		G_TYPE_POINTER
		);

	return GTK_TREE_MODEL (model);
}


static void
item_toggled (GtkCellRendererToggle *cell,gchar *path_str,gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *)data;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	GtkTreeIter iter;
	gboolean toggle_item;

	gint *column;

	column = (gint *)g_object_get_data (G_OBJECT (cell), "column");

	/* get toggled iter */
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, column, &toggle_item, -1);

	/* do something with the value */
	toggle_item ^= 1;

	/* set new value */
	gtk_tree_store_set (GTK_TREE_STORE (model), &iter, column,
		toggle_item, -1);

	gpointer p;
	gtk_tree_model_get (model, &iter, CAMSESSION_COLUMN, &p, -1);
	CamSession *s=(CamSession *)p;

	s->SetIncluded(toggle_item);

	/* clean up */
	gtk_tree_path_free (path);
}


static int sortcomparefunc(GtkTreeModel *model, GtkTreeIter  *a, GtkTreeIter  *b, gpointer userdata)
{
	char *name1,*name2;
	gtk_tree_model_get(model,a,LABEL_COLUMN,&name1,-1);
	gtk_tree_model_get(model,b,LABEL_COLUMN,&name2,-1);
	if(!name1)
		if(!name2)
			return(0);
		else
			return(-1);
	if(!name2)
		return(1);
	int r=strcmp(name1,name2);
	g_free(name1);
	g_free(name2);
	return(r);
}


void MainWindow::add_columns (GtkTreeView *treeview)
{
	gint col_offset;
	GtkCellRenderer *renderer;

	/* Active column */
	renderer = gtk_cell_renderer_toggle_new ();
	g_object_set (renderer, "xalign", 0.0, NULL);
	g_object_set_data (G_OBJECT (renderer), "column", (gint *)ACTIVE_COLUMN);
	
	g_signal_connect (renderer, "toggled", G_CALLBACK (item_toggled), model);
	
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
		-1, _("Include?"),
		renderer,
		"active",
		ACTIVE_COLUMN,
		"activatable",
		TRUE_COLUMN,
		NULL);


	/* column for names */
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (renderer, "xalign", 0.0, NULL);
	g_object_set (renderer, "xpad", 10, NULL);
	
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
	    -1, _("Date"),
	    renderer, "text",
	    LABEL_COLUMN,
	    NULL);


	/* Info column */
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (renderer, "xalign", 0.0, NULL);
	
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
		-1, _("Details"),
		renderer,"text",
		INFO_COLUMN,
		NULL);
}


static void quit( GtkWidget *widget,
                     gpointer   data )
{
    gtk_main_quit ();
}


void MainWindow::srcdir_changed(GtkFileChooser *chooser,gpointer userdata)
{
	MainWindow *mw=(MainWindow *)userdata;

	if(mw->ignoresrcdirchanged)
		return;

	mw->ignoresrcdirchanged=true;

	char *newsrc=gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (mw->sourcepathsel));
	if(mw->src && (strcmp(newsrc,mw->src)==0))
		cerr << "No change - ignoring" << endl;
	else
	{
		mw->ClearList();
		mw->SetSourceDir(newsrc);
	}
	mw->ignoresrcdirchanged=false;
}


#define RESPONSE_SETTINGS 1

MainWindow::MainWindow()
	: sessionlist(NULL), configfilename(NULL),src(NULL), window(NULL), ignoresrcdirchanged(false)
{
	GtkWidget *tmp;

	sessionlist=new CamSessionList(&config);

	bool rcfileexists=LoadSettings();

	sessionlist->SetExtensions();
	sessionlist->SetExclusions();

 	window=gtk_dialog_new_with_buttons(_("CamCardSync"),
		NULL,GtkDialogFlags(0),
		_("Settings..."),RESPONSE_SETTINGS,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		NULL);
	gtk_window_set_default_size (GTK_WINDOW (window), 650, 400);
	g_signal_connect (window, "destroy",
		G_CALLBACK (quit), &window);
	
//	GtkWidget *vbox = GTK_DIALOG(window)->vbox;
	GtkWidget *vbox=gtk_vbox_new(FALSE,8);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox),vbox,TRUE,TRUE,8);

	GtkWidget *hbox=gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,5);

	tmp=gtk_label_new(_("Copy files from: "));
	gtk_box_pack_start(GTK_BOX(hbox),tmp,FALSE,FALSE,5);

	sourcepathsel=gtk_file_chooser_button_new (_("Copy files to: "),GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);

	// Why do we receive multiple triggers from this signal?
	g_signal_connect(sourcepathsel,"selection-changed",G_CALLBACK(srcdir_changed),this);

	gtk_box_pack_start(GTK_BOX(hbox),sourcepathsel,TRUE,TRUE,0);
		
	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
	   GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
		GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	

	/* create model */
	model = create_model ();

	GtkTreeSortable *sortable=GTK_TREE_SORTABLE(model);
	gtk_tree_sortable_set_sort_func(sortable, LABEL_COLUMN, sortcomparefunc,0, NULL);
	gtk_tree_sortable_set_sort_column_id(sortable, LABEL_COLUMN, GTK_SORT_ASCENDING);

	/* create tree view */
	GtkWidget *treeview = gtk_tree_view_new_with_model (model);
	g_object_unref (model);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
	gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
			   GTK_SELECTION_MULTIPLE);
	
	add_columns (GTK_TREE_VIEW (treeview));
	
	gtk_container_add (GTK_CONTAINER (sw), treeview);

//  End of UI definition.
// Run dialog

	gtk_widget_show_all (window);
	while(gtk_events_pending())
		gtk_main_iteration_do(false);

	if(!rcfileexists)
		PrefsDialog(this);
}


MainWindow::~MainWindow()
{
	if(sessionlist)
		delete sessionlist;
	if(configfilename)
		free(configfilename);
	if(src)
		free(src);
}


void MainWindow::Run()
{
	bool done=false;
	while(!done)
	{
		gint result=gtk_dialog_run(GTK_DIALOG(window));
		switch(result)
		{
			case RESPONSE_SETTINGS:
				if(PrefsDialog(this))
				{
					// If prefs dialog returned true, then we
					// must rebuild the file list, since the extensions
					// could have changed.
					ClearList();
					ProgressBar pb("Scanning media for files...",true,window);
					const char *src=sessionlist->FindString("DefaultSourcePath");
					sessionlist->AddPath(src,&pb);
					PopulateList();
				}
				break;
			case GTK_RESPONSE_CANCEL:
				cerr << "Cancelled" << endl;
				done=true;
				break;
			case GTK_RESPONSE_OK:
				Go();
				cerr << "Clicked OK" << endl;
				done=true;
				break;
		}
	}
}


GtkWidget *MainWindow::GetWindow()
{
	return(window);
}


void MainWindow::ClearList()
{
	gtk_tree_store_clear(GTK_TREE_STORE(model));
	CamSession *cs;
	while((cs=sessionlist->FirstDay()))
		delete cs;	
}


void MainWindow::PopulateList()
{
	GtkTreeIter iter;
	/* add data to the tree store */
	const char *destdir=sessionlist->FindString("DestPath");
	sessionlist->SetIncludedDefaults(destdir);
	
	CamSession *s=sessionlist->FirstDay();
	while (s)
    {
		char *fmt=_("%.1f megabytes in %d image%c");
		char *infostring=(char *)malloc(strlen(fmt)+16);
		int mbc=(s->GetFileSize()*10+1023)/1024;
		float mbc2=mbc/10.0;
		int fc=s->GetFileCount();
		sprintf(infostring,fmt,mbc2,fc,(fc==1) ? ' ':'s');
		
		cerr << infostring << endl;

		gtk_tree_store_append (GTK_TREE_STORE(model), &iter, NULL);
		gtk_tree_store_set (GTK_TREE_STORE(model), &iter,
			ACTIVE_COLUMN, s->GetIncluded(),
			LABEL_COLUMN, s->GetDirName(),
			INFO_COLUMN, infostring,
			TRUE_COLUMN, TRUE,
			CAMSESSION_COLUMN, s,
			-1);
		free(infostring);
		s=s->NextDay();
    }
}


void MainWindow::SetSourceDir(const char *srcdir)
{
	if(src)
		free(src);

	if(srcdir)
		src=strdup(srcdir);
	else
	{
		srcdir=sessionlist->FindString("DefaultSourcePath");
		// If the default path doesn't exist, use a current dir instead.
		struct stat statbuf;
		if(stat(srcdir,&statbuf)!=0)
			src=g_get_current_dir();
		else
			src=strdup(srcdir);
	}

	cerr << "Setting path to: " << src << endl;

	// If the source dir is being changed as a result of a signal from the source
	// directory selector, then we don't want to update the directory selector.
	if(!ignoresrcdirchanged)
	{
		ignoresrcdirchanged=true;
		if(g_path_is_absolute(src))
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(sourcepathsel),src);
		else
		{
			gchar *cd=g_get_current_dir();
			gchar *path=g_build_filename(cd,src,NULL);
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(sourcepathsel),path);
			g_free(path);
			g_free(cd);
		}
	}

	ProgressBar pb("Scanning media for files...",true,window);
	sessionlist->SetString("DefaultSourcePath",src);
	sessionlist->AddPath(src,&pb);
	PopulateList();
	ignoresrcdirchanged=false;
}


void MainWindow::Go()
{
	const char *dst=sessionlist->FindString("DestPath");
	ImageCopyStats stats;
	if(dst)
	{
		ProgressBar pb(_("Copying files..."),true,window);		
		sessionlist->Copy(dst,&stats,&pb);
	}
	// Display stats
	float fsize=(10*stats.copiedsize+1023)/1024;
	fsize/=10.0;
	GtkWidget *dlg=gtk_message_dialog_new(GTK_WINDOW(window),GtkDialogFlags(0),
		GTK_MESSAGE_INFO,GTK_BUTTONS_OK,
		_("Copied %.1f mb in %d files.\n%d file%sskipped.\n"),fsize,stats.copied,stats.skipped,stats.skipped==1 ? " " : "s ");
	gtk_dialog_run(GTK_DIALOG(dlg));
}


void MainWindow::SaveSettings()
{
	config.SaveFile(configfilename);
}


bool MainWindow::LoadSettings()
{
	if(!configfilename)
		configfilename=substitute_homedir("$HOME/.camcardsyncrc");
	return(config.ParseFile(configfilename));
}

