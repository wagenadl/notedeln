#!/bin/sh

rm -rf eln.app eln.dmg
make -C src release
make -C webgrab release
make -C repairtoc release
cp webgrab/webgrab eln.app/Contents/MacOS
cp repairtoc/eln-repairtoc eln.app/Contents/MacOS
strip eln.app/Contents/MacOS/*
/Users/wagenaar/Qt5.2.1/5.2.1/clang_64/bin/macdeployqt eln.app -dmg -executable=eln.app/Contents/MacOS/webgrab -executable=eln.app/Contents/MacOS/eln-repairtoc
