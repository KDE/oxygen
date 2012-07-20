#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT `find . -name \*.cpp -o -name \*.h` -o $podir/liboxygenstyleconfig.pot
