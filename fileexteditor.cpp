#include <iostream>

#include <string.h>

#include <gtk/gtk.h>

#include "fileexteditor.h"

#include "config.h"
#include "gettext.h"
#define _(x) gettext(x)
#define N_(x) gettext_noop(x)

using namespace std;

#define NEWCELLTEXT _("Click to edit")


enum {
	CHANGED_SIGNAL,
	LAST_SIGNAL
};

static guint fileexteditor_signals[LAST_SIGNAL] = { 0 };

static void fileexteditor_class_init (FileExtEditorClass *klass);
static void fileexteditor_init (FileExtEditor *stpuicombo);


enum
{
  COLUMN_EXT,
  NUM_COLUMNS
};


static void cell_edited (GtkCellRendererText *cell, const gchar *path_string,
	const gchar *new_text, gpointer data)
{
	FileExtEditor *ee=(FileExtEditor *)data;
	GtkTreeModel *model = GTK_TREE_MODEL(ee->extlist);
	GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
	GtkTreeIter iter;

	gtk_tree_model_get_iter (model, &iter, path);

	gchar *old_text;

	gtk_tree_model_get (model, &iter, COLUMN_EXT, &old_text, -1);
	g_free (old_text);

	gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_EXT, g_strdup(new_text), -1);

	gtk_tree_path_free (path);
	fileexteditor_getextstring(ee);
}


static void add_item (GtkWidget *button, gpointer data)
{
	FileExtEditor *ee=(FileExtEditor *)data;
	GtkTreeIter iter;
	GtkTreeModel *model = ee->extlist;

	gtk_list_store_append (GTK_LIST_STORE (model), &iter);
	gtk_list_store_set (GTK_LIST_STORE (model), &iter,
		COLUMN_EXT, g_strdup(NEWCELLTEXT),-1);
	fileexteditor_getextstring(ee);
}


static void remove_item (GtkWidget *widget, gpointer data)
{
	FileExtEditor *ee=(FileExtEditor *)data;
	GtkTreeIter iter;
	GtkTreeView *treeview = GTK_TREE_VIEW(ee->modelview);
	GtkTreeModel *model = gtk_tree_view_get_model (treeview);
	GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
	{
		GtkTreePath *path;

		path = gtk_tree_model_get_path (model, &iter);
		gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

		gtk_tree_path_free (path);
	}
	fileexteditor_getextstring(ee);
}


GtkWidget*
fileexteditor_new (const char *listtitle)
{
	FileExtEditor *ob=FILEEXTEDITOR(g_object_new (fileexteditor_get_type (), NULL));
	gtk_box_set_spacing(GTK_BOX(ob),5);

	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
	                                    GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
	                                GTK_POLICY_AUTOMATIC,
	                                GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start (GTK_BOX (ob), sw, TRUE, TRUE, 0);

	/* create model */
	ob->extlist = GTK_TREE_MODEL(gtk_list_store_new (NUM_COLUMNS, G_TYPE_STRING));

	/* create tree view */
	ob->modelview = gtk_tree_view_new_with_model (ob->extlist);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (ob->modelview), TRUE);
	gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (ob->modelview)),
	                            GTK_SELECTION_SINGLE);


	GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
	g_object_set (renderer,"editable", TRUE,NULL);
	g_signal_connect (renderer, "edited",G_CALLBACK (cell_edited), ob);
	g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_EXT));

	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (ob->modelview),
		-1, listtitle, renderer,
		"text", COLUMN_EXT,
		NULL);

	g_object_unref (ob->extlist);

	gtk_container_add (GTK_CONTAINER (sw), ob->modelview);

	/* some buttons */
	GtkWidget *hbox = gtk_hbox_new (TRUE, 4);
	gtk_box_pack_start (GTK_BOX (ob), hbox, FALSE, FALSE, 0);

	GtkWidget *button = gtk_button_new_with_label (_("Add item"));
	g_signal_connect (button, "clicked",
	                G_CALLBACK (add_item), ob);
	gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);

	button = gtk_button_new_with_label (_("Remove item"));
	g_signal_connect (button, "clicked",
	                G_CALLBACK (remove_item), ob);
	gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);

	return(GTK_WIDGET(ob));
}


GType
fileexteditor_get_type (void)
{
	static GType stpuic_type = 0;

	if (!stpuic_type)
	{
		static const GTypeInfo fileexteditor_info =
		{
			sizeof (FileExtEditorClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) fileexteditor_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (FileExtEditor),
			0,
			(GInstanceInitFunc) fileexteditor_init,
		};
		stpuic_type = g_type_register_static (GTK_TYPE_VBOX, "FileExtEditor", &fileexteditor_info, (GTypeFlags)0);
	}
	return stpuic_type;
}


static void
fileexteditor_class_init (FileExtEditorClass *klass)
{
	fileexteditor_signals[CHANGED_SIGNAL] =
	g_signal_new ("changed",
		G_TYPE_FROM_CLASS (klass),
		GSignalFlags(G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION),
		G_STRUCT_OFFSET (FileExtEditorClass, changed),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}


static void
fileexteditor_init (FileExtEditor *ob)
{
}


void fileexteditor_setextstring(FileExtEditor *ob, const char *es)
{
	GtkTreeIter iter;
	GtkTreeModel *model = ob->extlist;

	char *e=strdup(es);
	char *ep=e;
	char *e2=e;
	while(*e2)
	{
		while((*e2) && (*e2!=':'))
			++e2;
		if(*e2)
		{
			*e2=0;

			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
			gtk_list_store_set (GTK_LIST_STORE (model), &iter,
				COLUMN_EXT, g_strdup(ep),-1);

			ep=e2=e2+1;
		}
		else
		{
			gtk_list_store_append (GTK_LIST_STORE (model), &iter);
			gtk_list_store_set (GTK_LIST_STORE (model), &iter,
				COLUMN_EXT, g_strdup(ep),-1);
		}
	}
	free(e);
}


char *fileexteditor_getextstring(FileExtEditor *ob)
{
	GtkTreeIter iter;
	GtkTreeModel *model = ob->extlist;

	gchar *exttext;

	int outlen=2;

	gtk_tree_model_get_iter_first(GTK_TREE_MODEL (model), &iter);
	do
	{
		gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, COLUMN_EXT, &exttext, -1);
		if(exttext)
		{
			if(strcmp(exttext,NEWCELLTEXT)!=0)
				outlen+=strlen(exttext)+1;
		}
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model),&iter));

	char *result=(char *)malloc(outlen);
	result[0]=0;
	char *outstr=result;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL (model), &iter);
	do
	{
		gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, COLUMN_EXT, &exttext, -1);
		if(exttext)
		{
			if(strcmp(exttext,NEWCELLTEXT)!=0)
			{
				if(result[0])
					strcat(outstr,":");
				if(exttext[0]=='.')
					++exttext;
				strcat(outstr,exttext);
			}
		}
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model),&iter));

	cerr << "Built output string: " << result << endl;

	return(result);
}
