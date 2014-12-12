#!/bin/bash

function build_qt() {
	mkdir -p build
	( cd build && \
		qmake ../ && \
		make )
}

function build_noqt() {
	mkdir -p build
	( cd build && \
		make -C ../simulator
		)
}


if [ "$1" == "" ] ; then
	OPT=qt
else
	OPT=$1
fi

case "$OPT" in 
	qt)
		build_qt
		;;
	noqt)
		build_noqt
		;;			
	*)
		echo "Usage: $0 (buildtype)"
		echo "   qt			build with QT"
		echo "   noqt		build without QT - command line tool only"
		echo "   help		show this message"
		exit 1
		;;
esac


