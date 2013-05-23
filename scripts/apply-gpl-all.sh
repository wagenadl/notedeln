#!/bin/zsh

cd `dirname $0`
cd ../src

for a in */*.cpp */*.H; ../scripts/apply-gpl.h $a
