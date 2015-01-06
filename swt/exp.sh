#!/bin/sh

for i in `seq 1 233`; do
    ./swttest -n $i -c 0 -l 50 -h 100 >> swt_log.txt
    cp output.jpg image/result/swt_0_$i.jpg
    ./swttest -n $i -c 1 -l 50 -h 100 >> swt_log.txt
    cp output.jpg image/result/swt_1_$i.jpg
done

