#!/bin/bash

if [ "$1" == "" ] ; then
	echo "usage: $0 input_file.dsf"
	exit 1
fi

DDIR=`dirname $1`

cat "$1" | while read -r line ; do
	IFS="," read -r -a args <<< "$line"

	IN="${args[0]}"
	NAME=`basename $IN | sed -e 's/.dat//g'`
	NOW=`date`

	if [ ! -f $DDIR/$NAME.done ] ; then

		echo "$NOW Executing ${args[0]}" > $DDIR/$NAME.started

		displace -f "${args[3]}" -f2 "${args[2]}" -s "${args[1]}" -i "${args[4]}" -p 1 -o 1 -e 0 -v 0 -V 2 --num_threads "${args[5]}" --without-gnuplot &> $DDIR/$NAME.out
		echo "$NOW Completed ${args[0]}" >> $DDIR/$NAME.started
		mv $DDIR/$NAME.started $DDIR/$NAME.done
	else
		echo "$NOW Skipped ${args[0]}" >> $DDIR/$NAME.done
	fi
done




