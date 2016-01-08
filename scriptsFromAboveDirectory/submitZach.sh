#!/bin/bash
rm -rf /star/u/zamiller/run14NPE/LocalLibraries_hists.*
for RUN in `cat List/testListJan4.list`
do
     echo $RUN
     #cd Submit/$day/$RUN
     #ln -s ../../../AuAu200.xml
     star-submit-template -template bingchuSubmit.xml -entities runnumber=$RUN
done
