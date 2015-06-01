ifdef DESTDIR
INSTALLPATH = $(DESTDIR)/usr
SHAREPATH = $(DESTDIR)/usr/share
else
INSTALLPATH = /usr/local
SHAREPATH = /usr/local/share
endif

DOCPATH = $(SHAREPATH)/doc/eln

all: SRC WEBGRAB

clean:
	+make -C src clean
	+make -C webgrab clean

SRC:
	tools/updatesources.sh
	( cd src; qmake-qt4 )
	+make -C src release

WEBGRAB:
	( cd webgrab; qmake-qt4 )
	+make -C webgrab release

install: all
	install -d $(INSTALLPATH)/bin
	install -d $(INSTALLPATH)/man/man1
	install -d $(SHAREPATH)/pixmaps
	install -d $(SHAREPATH)/applications
	install -d $(DOCPATH)
	install src/eln $(INSTALLPATH)/bin/eln
	install webgrab/webgrab $(INSTALLPATH)/bin/webgrab
	install doc/eln.1 $(INSTALLPATH)/man/man1/eln.1
	install doc/webgrab.1 $(INSTALLPATH)/man/man1/webgrab.1
	install src/eln.png $(SHAREPATH)/pixmaps/eln.png
	install src/eln.desktop $(SHAREPATH)/applications/eln.desktop
	install doc/userguide.pdf $(DOCPATH)/userguide.pdf
	install LICENSE $(DOCPATH)/LICENSE
	install README $(DOCPATH)/README
	install CHANGELOG $(DOCPATH)/CHANGELOG
	install GPL-3.0.gz $(DOCPATH)/GPL-3.0.gz
	install src/App/fonts/ubuntu-font-licence-1.0.txt.gz $(DOCPATH)/ubuntu-font-licence-1.0.txt.gz

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

.PHONY: SRC WEBGRAB

