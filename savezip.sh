#!/bin/sh

if [ $# != 1 ]; then
    echo "Usage: $0 date"
    exit 0
fi


cd res
for f in *.txt; do 
    echo $f
    nkf -Lw $f > buf
    cat buf | sort | uniq > $f
done
zip res_$1.zip *.txt

cd ../
# tar zcf det_$1.tar.gz res/*.txt images/save
#
# mv det_$1.tar.gz ../savedata/TextDetection/
