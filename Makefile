# Makefile - Part of NotedELN, (C) Daniel Wagenaar 2021

# Actual build process is through cmake, but typing "make"
# is easier than "cmake -S . -B build" etc.

######################################################################
# Linux and Mac stuff
release: prep-release
	+cmake --build build --config Release

prep-release:
	+cmake -S . -B build -DCMAKE_BUILD_TYPE=Release  

debug: prep-debug
	+cmake --build build-debug --config Debug

prep-debug:
	+cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug 

clean:; rm -rf build build-debug

# linux only:
deb:	release
	(cd build; cpack )

# max only:
dmg:	release
	+make -C build dmg

tar:;	git archive -o ../notedeln.tar.gz --prefix=notedeln/ HEAD

######################################################################
.PHONY: release prep-release debug prep-debug clean tar deb dmg

