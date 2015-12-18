#!/bin/bash
date

if [ $# -lt 1 ]; then
    echo "not enough arguements"
    echo "./submitByRun.sh <#>"
	exit
fi

if [ -f runlist_now ]; then
    rm runlist_now
fi

picoDir=/gpfs01/star/pwg_tasks/hf02/picodsts/Run14/AuAu/200GeV/physics2/P15ic

ls -d ${picoDir}/*/* | cut -d '/' -f 13 >& runlist_now
if [ -f runlist_add ]; then
	rm runlist_add
fi
cat runlist_now runlist_old | sort | uniq -u > runlist_add

if [ ! -d filelist ]; then
    mkdir filelist
fi

if [ ! -d out ]; then
    mkdir out
fi
if [ ! -d log ]; then
    mkdir log
fi


dir=$(pwd)
ver=$1
echo $dir
if [ ! -d ${dir}/out/out_$1 ]; then
	mkdir ${dir}/out/out_$1
fi
if [ ! -d ${dir}/log/log_$1 ]; then
	mkdir ${dir}/log/log_$1
fi

mkdir submit
cp $dir/submit.xml submit/.
cd submit/
 
 
#check old runs
for oldrun in `cat $dir/runlist_old`
do

	day=${oldrun:2:3}
	year=$[ ${oldrun:0:2}-1 ]
	echo "$oldrun $day 20$year"
	num1=`grep -c picoDst.root ${dir}/filelist/pico_${oldrun}.list`
	num2=`ls ${picoDir}/${day}/${oldrun}/*.picoDst.root | grep -c picoDst.root`
	echo $num1 $num2
	if [ $num1 -lt $num2 ]; then
		echo "resubmitting run ${oldrun} ..."
		rm $dir/filelist/pico_${oldrun}.list
		rm $dir/out/out_$1/run_${oldrun}.*
		rm $dir/log/log_$1/anaTree_${oldrun}.*
		ls ${picoDir}/${day}/${oldrun}/*.picoDst.root --color=none >& $dir/filelist/pico_${oldrun}.list
		star-submit-template -template submit.xml -entities nEvents=10000000,filelist=filelist/pico_${oldrun}.list,path=${dir},outfile=run_${oldrun}.anaTree.root,outpath=${dir},libver=$1,jobid=${oldrun}
	fi
done

for run in `cat $dir/runlist_add`
do

	day=${run:2:3}
	year=$[ ${run:0:2}-1 ]
	echo $run $day $year
    if [ -f $dir/filelist/pico_${run}.list ]; then
    	rm $dir/filelist/pico_${run}.list
    fi
    ls ${picoDir}/${day}/${run}/*.picoDst.root --color=none >& $dir/filelist/pico_${run}.list
    star-submit-template -template submit.xml -entities nEvents=10000000,filelist=filelist/pico_${run}.list,path=${dir},outfile=run_${run}.anaTree.root,outpath=${dir},libver=$1,jobid=${run}
done

cat $dir/runlist_add >> $dir/runlist_old
