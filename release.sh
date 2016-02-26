#!/bin/sh

set -e

VERSION=`git describe --abbrev=0 --always --tags`
NAME=displace-$VERSION
TEMP=../temp-src
SRCPATH=$TEMP/$NAME

mkdir -p $SRCPATH
rsync -rav --progress --exclude .git --exclude .qt.conf --exclude \*~ --exclude \*.tar.gz . $SRCPATH
( cd $TEMP && tar zcvf $NAME.tar.gz $NAME )

mv $TEMP/$NAME.tar.gz .
rm -rf $TEMP

ls -l $NAME.tar.gz


