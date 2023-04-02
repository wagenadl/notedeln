# Makefile - Part of NotedELN, (C) Daniel Wagenaar 2021

# This Makefile just documents some generally useful actions.
# Actual build process is through cmake.

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

deb:	release
	(cd build; cpack )

tar:;	git archive -o ../notedeln.tar.gz --prefix=notedeln/ HEAD

######################################################################
.PHONY: release prep-release debug prep-debug clean tar deb

