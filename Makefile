INSTALLPATH=/usr/local/bin

all: SRC REPAIRTOC WEBGRAB

SRC:
	scripts/updatesources.sh
	( cd src; qmake )
	make -C src release

REPAIRTOC:
	( cd repairtoc; qmake )
	make -C repairtoc release

WEBGRAB:
	( cd webgrab; qmake )
	make -C webgrab release

inst: all
	install src/eln $(INSTALLPATH)/eln
	install repairtoc/eln-repairtoc $(INSTALLPATH)/eln-repairtoc
	install webgrab/webgrab $(INSTALLPATH)/webgrab

deb:	all
	debuild -us -uc -Idebug -Irelease -I.bzr -Ieln -Ieln_debug -Iwebgrab/webgrab -Irepairtoc/eln-repairtoc -Ifrontimage.xcf -IMakefile.Debug -IMakefile.Release

macclean:; rm -rf eln.app eln.dmg

macapp: all
	cp webgrab/webgrab eln.app/Contents/MacOS
	cp repairtoc/eln-repairtoc eln.app/Contents/MacOS
	strip eln.app/Contents/MacOS/*
	cp src/App/elnmac.sh eln.app/Contents/MacOS/
	chmod a+x eln.app/Contents/MacOS/elnmac.sh

macdmg: macapp
	/Users/wagenaar/Qt5.2.1/5.2.1/clang_64/bin/macdeployqt eln.app -dmg -executable=eln.app/Contents/MacOS/webgrab -executable=eln.app/Contents/MacOS/eln-repairtoc

.PHONY: SRC WEBGRAB REPAIRTOC

