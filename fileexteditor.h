#ifndef __FILEEXTEDITOR_H__
#define __FILEEXTEDITOR_H__

#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>

G_BEGIN_DECLS

#define FILEEXTEDITOR_TYPE			(fileexteditor_get_type())
#define FILEEXTEDITOR(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), FILEEXTEDITOR_TYPE, FileExtEditor))
#define FILEEXTEDITOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), FILEEXTEDITOR_TYPE, FileExtEditorClass))
#define IS_FILEEXTEDITOR(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), FILEEXTEDITOR_TYPE))
#define IS_FILEEXTEDITOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), FILEEXTEDITOR_TYPE))

typedef struct _FileExtEditor FileExtEditor;
typedef struct _FileExtEditorClass FileExtEditorClass;


struct _FileExtEditor
{
	GtkVBox	box;
	GtkWidget *modelview;
	GtkTreeModel *extlist;
};


struct _FileExtEditorClass
{
	GtkVBoxClass parent_class;

	void (*changed)(FileExtEditor *sig);
};

GType fileexteditor_get_type (void);
GtkWidget* fileexteditor_new (const char *heading="");
// Returns a newly-allocated string which the caller should g_free() later.
char *fileexteditor_getextstring(FileExtEditor *ee);
void fileexteditor_setextstring(FileExtEditor *ee,const char *exts);

G_END_DECLS

#endif /* __FILEEXTEDITOR_H__ */
