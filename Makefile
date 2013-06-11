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

install: all
	install src/eln $(INSTALLPATH)/eln
	install repairtoc/repairtoc $(INSTALLPATH)/eln-repairtoc
	install webgrab/webgrab $(INSTALLPATH)/webgrab

deb:	all
	debuild -us -uc
