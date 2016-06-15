#!/bin/sh

set -e

help() {
cat << _EOF
Usage: release [-v version] [-h] [suffix]

	-v version	Define version string. If not used,
			version is desumed from git tags
	-h		Show this help
	suffix		Append suffix to version string
_EOF
}

VERSION=`git describe --abbrev=0 --always --tags`

case "$1" in
	-v) 
		shift
		VERSION="$1"
		shift
		;;
	-h)
		help
		exit 1
		;;
esac

MOD="$1"

NAME=displace-$VERSION$MOD
TEMP=../temp-src
SRCPATH=$TEMP/$NAME

mkdir -p $SRCPATH
rsync -rav --progress --exclude .git --exclude .qt.conf --exclude \*~ --exclude \*.tar.gz --exclude build --exclude 3rd-party . $SRCPATH
( cd $TEMP && tar zcvf $NAME.tar.gz $NAME )

mv $TEMP/$NAME.tar.gz .
rm -rf $TEMP

ls -l $NAME.tar.gz


