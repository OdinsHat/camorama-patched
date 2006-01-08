NULL=
INCLUDES = \
   -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DDATADIR=\""$(datadir)"\"    \
	-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\" \
		   $(PACKAGE_CFLAGS)
			

bin_PROGRAMS = camorama

BUILT_SOURCES    = camorama-stock-images.h
camorama_SOURCES = \
	camorama-stock-items.c	\
	camorama-stock-items.h	\
	main.c		\
	v4l.c		\
	filter.c	\
	callbacks.c	\
	support.c	\
	fileio.c	\
	eggtrayicon.c	\
	v4l.h		\
	callbacks.h	\
	support.h	\
	interface.h	\
	fileio.h	\
	font_6x11.h	\
	filter.h	\
	eggtrayicon.h	\
	$(BUILT_SOURCES)\
	$(NULL)
camorama_LDADD = $(PACKAGE_LIBS)

DISTCLEANFILES=$(BUILT_SOURCES)

camorama-stock-images.h:
	gdk-pixbuf-csource --raw --build-list \
		camorama_webcam_16 ../pixmaps/camorama-webcam-16.png \
		camorama_webcam_24 ../pixmaps/camorama-webcam-24.png \
	> $@ || (rm -f $@)

