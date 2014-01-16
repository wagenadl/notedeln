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

