ifdef DESTDIR
INSTALLPATH = $(DESTDIR)/usr
SHAREPATH = $(DESTDIR)/usr/share
else
INSTALLPATH = /usr/local
SHAREPATH = /usr/local/share
endif

DOCPATH = $(SHAREPATH)/doc/eln

all: SRC WEBGRAB DOC

clean:
	+make -C src clean
	+make -C webgrab clean
	+make -C doc clean

SRC:
	tools/updatesources.sh
	( cd src; qmake-qt4 || qmake )
	+make -C src release

WEBGRAB:
	( cd webgrab; qmake-qt4 || qmake )
	+make -C webgrab release

install: all
	install -d $(INSTALLPATH)/bin
	install -d $(SHAREPATH)/man/man1
	install -d $(SHAREPATH)/pixmaps
	install -d $(SHAREPATH)/applications
	install -d $(SHAREPATH)/icons/gnome/48x48/mimetypes
	install -d $(SHAREPATH)/mime/packages
	install -d $(DOCPATH)
	install build/eln $(INSTALLPATH)/bin/eln
	install build/webgrab $(INSTALLPATH)/bin/webgrab
	cp doc/eln.1 $(SHAREPATH)/man/man1/eln.1
	cp doc/webgrab.1 $(SHAREPATH)/man/man1/webgrab.1
	cp src/App/eln.png $(SHAREPATH)/pixmaps/eln.png
	cp src/eln.xpm $(SHAREPATH)/pixmaps/eln.xpm
	cp src/App/eln.png $(SHAREPATH)/icons/gnome/48x48/mimetypes/application-eln-book.png
	# gtk-update-icon-cache $(SHAREPATH)/icons/gnome || true

	cp src/eln.xml $(SHAREPATH)/mime/packages/eln.xml
	# update-mime-database $(SHAREPATH)/mime/ || true

	install src/eln.desktop $(SHAREPATH)/applications/eln.desktop
	cp doc/userguide.pdf $(DOCPATH)/userguide.pdf
	cp README $(DOCPATH)/readme
	gzip -9 $(DOCPATH)/readme
	cp CHANGELOG $(DOCPATH)/changelog
	gzip -9 $(DOCPATH)/changelog
	install src/Gui/fonts/ubuntu-font-licence-1.0.txt.gz $(DOCPATH)/ubuntu-font-licence-1.0.txt.gz

DOC:;	+make -C doc

tar: all
	git archive -o ../eln.tar.gz --prefix=eln/ HEAD

macclean:; rm -rf eln.app eln.dmg

macapp: all
	cp webgrab/webgrab eln.app/Contents/MacOS
	strip eln.app/Contents/MacOS/*
	cp src/App/elnmac.sh eln.app/Contents/MacOS/
	chmod a+x eln.app/Contents/MacOS/elnmac.sh

macdmg: macapp
	/Users/wagenaar/Qt5.2.1/5.2.1/clang_64/bin/macdeployqt eln.app -dmg -executable=eln.app/Contents/MacOS/webgrab 

.PHONY: SRC WEBGRAB DOC all clean tar macclean macapp macdmg

