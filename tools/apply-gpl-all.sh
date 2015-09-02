#!/bin/zsh

cd `dirname $0`
cd ../src

for a in */*.cpp */*.h; ../tools/apply-gpl.sh $a
