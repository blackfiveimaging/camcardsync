# Makefile.am
# AMR 28/04/07

SUBDIRS = po

localedir=$(datadir)/locale

AM_CXXFLAGS = -DLOCALEDIR=\"$(localedir)\" -Wall $(GTK2_CFLAGS) $(HAL_CFLAGS)

bin_PROGRAMS = camcardsync

camcardsync_SOURCES = 		\
	camcardsync.cpp	\
	camsession.cpp	\
	camsession.h	\
	configdb.cpp	\
	configdb.h	\
	daystamp.h	\
	fileext.cpp	\
	fileext.h	\
	exclusions.cpp \
	exclusions.h \
	fileexteditor.cpp	\
	fileexteditor.h	\
	gettext.h	\
	halgetmountpoint.cpp	\
	halgetmountpoint.h	\
	imagefile.cpp	\
	imagefile.h	\
	mainwindow.cpp	\
	mainwindow.h	\
	pathsupport.cpp	\
	pathsupport.h	\
	prefsdialog.cpp	\
	prefsdialog.h	\
	progress.h	\
	progresstext.h \
	progressbar.cpp	\
	progressbar.h

camcardsync_LDADD = \
	$(LIBINTL) $(LIBM_LIBS) $(GETOPT_LIBS) $(JPEG_LIBS) $(PNM_LIBS) $(TIFF_LIBS) $(LCMS_LIBS) $(GP_LIBS) $(GTK2_LIBS) $(HAL_LIBS)


#desktopdir = $(datadir)/applications
#desktop_DATA = fotoprint.desktop

#icondir = $(datadir)/icons/hicolor/48x48/apps/
#icon_DATA = fotoprint.png


EXTRA_DIST = COPYING README TODO NEWS

