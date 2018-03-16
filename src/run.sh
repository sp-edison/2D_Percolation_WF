#!/bin/bash

INPUT=""
while getopts 'i:' opt; do
	case $opt in
		'i')
			echo "input inserted"
			INPUT="$OPTARG";
			echo $INPUT
		;;
	esac
done
if [ "${INPUT}" != "" ]; then
	./2d_Perc -i $INPUT && cp $INPUT ./result/input.txt
	EXITCODE=$?;
	if [ "$?" == "0" ]; then
		head -1 ./result/lattice.txt | grep 7 | wc -l > ./result/spanning.txt
	fi
fi
