#!/bin/bash

date

#/star/u/huangbc/data01/pico/anaTree/20151125_v4/prod/out/out_mb/15136014/st_physics_adc_15136014_raw_4000036.anaTree.root
anaDir=/star/u/huangbc/data01/pico/anaTree/20151125_v4/prod/out/out_mb

currentDir=$(pwd)
echo $pwd

if [ ! -d listfile ]; then
	mkdir listfile
fi	

for runid in `cat $currentDir/runidlist`
do 
	ls $anaDir/${runid}/*.root --color=none >& $currentDir/listfile/input_${runid}.list

	ifile=$currentDir/listfile/input_${runid}.list
	echo ${ifile}

	nifile=`cat $currentDir/listfile/input_${runid}.list | wc -l`
    echo $nifile
	
    root4star -l -b -q 'myhadd.cpp('$nifile',"'${ifile}'",'$runid')';

done

