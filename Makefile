# This makefile is for Linux and MacOS building
# Windows uses QCreator.

# Unix installation
ifdef DESTDIR
  # Debian uses this
  INSTALLPATH = $(DESTDIR)/usr
  SHAREPATH = $(DESTDIR)/usr/share
else
  INSTALLPATH = /usr/local
  SHAREPATH = /usr/local/share
endif

UNAME=$(shell uname -s)

ifeq (, $(shell which qmake-qt5))
  QMAKE=qmake
else
  QMAKE=qmake-qt5
endif

ifeq ($(UNAME),Linux)
  # Linux
  SELECTQT=QT_SELECT=5
else
  ifeq ($(UNAME),Darwin)
    # Mac OS
    QROOT=/Users/wagenaar/Qt-5.7/5.7
    QROOT=/Users/wagenaar/Qt-5.12/5.12.1
    QBINPATH=$(QROOT)/clang_64/bin
    QMAKE=$(QBINPATH)/qmake
  else
    $(error Unknown operating system. This Makefile is for Mac or Linux.)
  endif
endif

DOCPATH = $(SHAREPATH)/doc/eln

# Linux and Mac building
all: src webgrab man userguide

update:
	tools/updatesources.sh

clean:
	+rm -rf build
	+rm -rf build-webgrab
	+rm -rf build-doc

src: prep
	+make -C build release

prep:
	mkdir -p build
	rm -f build/*/BuildDate.o
	( cd build; $(SELECTQT) $(QMAKE) ../src/eln.pro )

webgrab: webgrabprep
	+make -C build-webgrab release

webgrabprep:
	mkdir -p build-webgrab
	( cd build-webgrab;  $(SELECTQT) $(QMAKE) ../webgrab/webgrab.pro )

# Unix installation
install: all install-userguide
	install -d $(INSTALLPATH)/bin
	install -d $(SHAREPATH)/man/man1
	install -d $(SHAREPATH)/pixmaps
	install -d $(SHAREPATH)/applications
	install -d $(SHAREPATH)/icons/gnome/24x24/mimetypes
	install -d $(SHAREPATH)/icons/gnome/48x48/mimetypes
	install -d $(SHAREPATH)/icons/gnome/16x16/mimetypes
	install -d $(SHAREPATH)/icons/gnome/32x32/mimetypes
	install -d $(SHAREPATH)/icons/gnome/64x64/mimetypes
	install -d $(SHAREPATH)/icons/gnome/128x128/mimetypes
	install -d $(SHAREPATH)/mime/packages
	install -d $(DOCPATH)
	install build/eln $(INSTALLPATH)/bin/eln
	install build-webgrab/webgrab $(INSTALLPATH)/bin/webgrab
	cp build-doc/eln.1 $(SHAREPATH)/man/man1/eln.1
	cp build-doc/webgrab.1 $(SHAREPATH)/man/man1/webgrab.1
	cp src/App/eln.png $(SHAREPATH)/pixmaps/eln.png
	cp src/eln.xpm $(SHAREPATH)/pixmaps/eln.xpm
	cp src/App/eln.iconset/icon_16x16.png $(SHAREPATH)/icons/gnome/16x16/mimetypes/application-eln-book.png
	cp src/App/eln.iconset/icon_24x24.png $(SHAREPATH)/icons/gnome/24x24/mimetypes/application-eln-book.png
	cp src/App/eln.iconset/icon_32x32.png $(SHAREPATH)/icons/gnome/32x32/mimetypes/application-eln-book.png
	cp src/App/eln.iconset/icon_48x48.png $(SHAREPATH)/icons/gnome/48x48/mimetypes/application-eln-book.png
	cp src/App/eln.iconset/icon_64x64.png $(SHAREPATH)/icons/gnome/64x64/mimetypes/application-eln-book.png
	cp src/App/eln.iconset/icon_128x128.png $(SHAREPATH)/icons/gnome/128x128/mimetypes/application-eln-book.png
	cp src/eln.xml $(SHAREPATH)/mime/packages/eln.xml
	install src/eln.desktop $(SHAREPATH)/applications/eln.desktop
	cp README.md $(DOCPATH)/readme
	gzip -9 $(DOCPATH)/readme
	cp CHANGELOG $(DOCPATH)/changelog
	gzip -9 $(DOCPATH)/changelog
	install src/Gui/fonts/ubuntu-font-licence-1.0.txt.gz $(DOCPATH)/ubuntu-font-licence-1.0.txt.gz
#       gtk-update-icon-cache $(SHAREPATH)/icons/gnome || true
#       update-mime-database $(SHAREPATH)/mime/ || true

man:
	mkdir -p build-doc
	cp doc/Makefile build-doc/
	+make -C build-doc eln.1 webgrab.1

userguide:
	mkdir -p build-doc
	cp doc/Makefile build-doc/
	+make -C build-doc userguide.pdf

install-userguide: doc
	install -d $(DOCPATH)
	cp build-doc/userguide.pdf $(DOCPATH)/userguide.pdf

# Tar preparation
tar: all
	git archive -o ../eln.tar.gz --prefix=eln/ HEAD

# Mac installation
macclean:; rm -rf eln.app eln.dmg

macapp: SRC WEBGRAB 
	mkdir -p eln.app/Contents/MacOS
	cp build-webgrab/webgrab eln.app/Contents/MacOS
	cp src/App/elnmac.sh eln.app/Contents/MacOS/
	chmod a+x eln.app/Contents/MacOS/elnmac.sh

macdmg: macapp
	$(QBINPATH)/macdeployqt eln.app -dmg -executable=eln.app/Contents/MacOS/webgrab -executable=eln.app/Contents/MacOS/eln

.PHONY: src webgrab all clean tar macclean macapp macdmg man userguide \
        install install-userguide prep webgrabprep

