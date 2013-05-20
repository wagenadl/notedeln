all:
	make -C src
	make -C repairtoc
	make -C webgrab

deb:	all
	debuild -us -uc
