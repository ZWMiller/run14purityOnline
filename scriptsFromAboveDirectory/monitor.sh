#!/bin/bash

#while true
#do

nfmu=$(get_file_list.pl -keys filename -cond production=P15ic,filetype=daq_reco_mudst,filename~st_physics,storage!=hpss -limit 0 -onefile | wc -l)

nevtsnfs=$(get_file_list.pl -keys 'sum(events)' -cond production=P15ic,filetype=daq_reco_mudst,filename~st_physics,storage=nfs -limit 0 -distinct)
nevtslocal=$(get_file_list.pl -keys 'sum(events)' -cond production=P15ic,filetype=daq_reco_mudst,filename~st_physics,storage=local -limit 0 -distinct)
nevtstot=$(get_file_list.pl -keys 'sum(events)' -cond production=P15ic,filetype=daq_reco_mudst,filename~st_physics,storage!=hpss -limit 0 -distinct)

nfmuq=$(cat PicoList/*/*MuDst.lis.sum | wc -l)
nfmus=$(cat MuDstToBeSubmitted*.lis | wc -l)

# number of picoDst files cleaned
nfpic=$(cat PicoList/*/*clean* | wc -l)
# number of picoDst files in production directory
nfpid=$(find PicoProd/ -maxdepth 3 -name "*.picoDst.root" | wc -l)
# number of picoDst files in PWG
nfpip=$(find pwg/ -maxdepth 3 -name "*.picoDst.root" | wc -l)

echo " Number of events - NFS = " $nevtsnfs
echo " Number of events - local = " $nevtslocal
echo " Number of events - NFS + local = " $nevtstot

echo " Number of MuDst files from FileCataLog = " $nfmu
echo " Number of MuDst files processed = " $nfmuq
echo " Number of MuDst files to be submitted = " $nfmus
echo " Number of removed picoDst files = " $nfpic
echo " Number of picoDst files in PicoProd = " $nfpid
echo " Number of picoDst files in PWG = " $nfpip

#date
#echo " wait for another 6 hours to check again"
#sleep 6h

#done
