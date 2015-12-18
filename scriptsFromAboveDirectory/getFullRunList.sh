#!/bin/bash

for day in `cat List/AuAu200_day.lis`
do
  echo ${day}
  get_file_list.pl -keys 'runnumber' -cond 'daynumber='${day}',filename~st_physics,collision=auau200,runnumber[]15000000-15170000,tpx=1,tof=1,trgsetupname=AuAu_200_production_2014 || AuAu_200_production_mid_2014 || AuAu_200_production_low_2014 || AuAu_200_production_high_2014' -limit 0 -distinct | sort >> List/day${day}.lis

done
