#!/bin/bash
rm -rf /star/u/zamiller/run14NPE/LocalLibraries_hists.*
for RUN in `cat List/fullRunList_Bingchu.list`
#for RUN in `cat List/testListShort.list`
do
     echo $RUN
     #cd Submit/$day/$RUN
     #ln -s ../../../AuAu200.xml
     star-submit-template -template bingchuSubmit2.xml -entities runnumber=$RUN
done
