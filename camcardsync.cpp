#include <iostream>

#include <locale.h>
#include <gtk/gtk.h>

#include "config.h"

#include "camsession.h"
#include "configdb.h"
#include "halgetmountpoint.h"
#include "mainwindow.h"

#include "config.h"
#include "gettext.h"
#define _(x) gettext(x)
#define N_(x) gettext_noop(x)

using namespace std;

#define HALPATH "/org/freedesktop/Hal"

int main(int argc,char **argv)
{
	gtk_init(&argc,&argv);
	char *path=NULL;

	cerr << "Setting up gettext... " << PACKAGE << ", " << LOCALEDIR << endl;
	setlocale(LC_ALL,"");
	bindtextdomain(PACKAGE,LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);


	if(argc==2)
	{
#ifdef HAVE_HAL
		if(strncasecmp(argv[1],HALPATH,strlen(HALPATH))==0)
		{
			path=HALGetMountPoint(argv[1]);
		}
		else
#endif
			path=strdup(argv[1]);
	}

	MainWindow mw;

	mw.SetSourceDir(path);  // If null, a default will be used.
	if(path)
	{
		free(path);
	}
	mw.Run();
	return(0);
}

