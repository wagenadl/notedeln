# Makefile - Part of NotedELN, (C) Daniel Wagenaar 2021

# This Makefile just documents some generally useful actions.
# Actual build process is through cmake.

######################################################################
# Linux stuff
release: prep-release
	( cd build; cmake --build . )

prep-release:
	mkdir -p build
	( cd build; cmake -DCMAKE_BUILD_TYPE=Release  .. )

debug: prep-debug
	( cd build-debug; cmake --build . )

prep-debug:
	mkdir -p build-debug
	( cd build-debug; cmake -DCMAKE_BUILD_TYPE=Debug .. )

clean:; rm -rf build build-debug

tar:;	git archive -o ../notedeln.tar.gz --prefix=notedeln/ HEAD

######################################################################
# Mac OS stuff - to be updated
macclean:; rm -rf eln.app eln.dmg

macapp: SRC WEBGRAB 
	mkdir -p eln.app/Contents/MacOS
	cp build-webgrab/webgrab eln.app/Contents/MacOS
	cp src/App/elnmac.sh eln.app/Contents/MacOS/
	chmod a+x eln.app/Contents/MacOS/elnmac.sh

macdmg: macapp
	$(QBINPATH)/macdeployqt eln.app -dmg -executable=eln.app/Contents/MacOS/webgrab -executable=eln.app/Contents/MacOS/eln

######################################################################
.PHONY: release prep-release debug prep-debug clean tar macclean macapp macdmg 

