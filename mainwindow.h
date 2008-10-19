#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtkwindow.h>
#include <gtk/gtktreemodel.h>

#include "configdb.h"
#include "camsession.h"

class CamSessionList;
class MainWindow
{
	public:
	MainWindow();
	~MainWindow();
	GtkWidget *GetWindow();
	void SetSourceDir(const char *src=NULL);
	void Run();
	void ClearList();
	void PopulateList();
	void Go();
	void SaveSettings();
	bool LoadSettings();
	CamSessionList *sessionlist;
	ConfigFile config;
	protected:
	void add_columns(GtkTreeView *treeview);
	char *configfilename;
	char *src;
	GtkWidget *window;
	GtkWidget *sourcepathsel;
	GtkTreeModel *model;
	static void srcdir_changed(GtkFileChooser *chooser,gpointer user_data);
	bool ignoresrcdirchanged;
};

#endif
