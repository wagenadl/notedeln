#!/bin/sh

export PATH=$PATH:/usr/local/bin

exec "`dirname \"$0\"`/eln" $@
