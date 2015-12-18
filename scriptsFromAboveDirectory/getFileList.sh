#!/bin/bash

#get_file_list.pl -keys 'runnumber' -cond 'production=P11id, trgsetupname=AuAu27_production_2011,filetype=daq_reco_MuDst,storage=nfs' -distinct -limit 0
#get_file_list.pl -keys 'path,filename' -cond 'production=P11id, trgsetupname=AuAu27_production_2011,filetype=daq_reco_MuDst,storage!=hpss' -delim / -distinct -limit 0
#get_file_list.pl -keys 'path,filename' -cond 'filetype=daq_reco_MuDst,storage!=hpss,filename~st_mon' -delim / -distinct

#get_file_list.pl -keys 'path,filename' -cond 'production=P11id,trgsetupname=AuAu200_production_2011,filetype=daq_reco_mudst,filename~st_physics,storage=NFS' -limit 0 -delim / -distinct
#get_file_list.pl -keys 'path,filename' -cond 'production=P11id,trgsetupname=AuAu200_production_2011,filetype=daq_reco_mudst,filename~st_physics,storage=local' -limit 0 -delim / -distinct
#get_file_list.pl -keys 'path,filename' -cond 'production=P11id,trgsetupname=AuAu200_production_2011,filetype=daq_reco_mudst,filename~st_physics,storage!=HPSS' -limit 0 -delim / -distinct
get_file_list.pl -keys 'runnumber' -cond 'daynumber[]075-166,filename~st_physics,storage!=HPSS,collision=auau200,runnumber[]15000000-15170000,tpx=1,tof=1,trgsetupname=AuAu_200_production_2014 || AuAu_200_production_mid_2014 || AuAu_200_production_low_2014 || AuAu_200_production_high_2014' -limit 0
