#!/bin/sh

if [ $# != 1 ]; then
    echo "Usage : $0 exp_id" 
    exit 0
fi
EXPID=$1
DIR=/home/hinami/work/TextDetection/
# scp -i ~/.ssh/id_rsa.sakura $DIR/log/log.txt root@133.242.130.178:/var/www/phpraw/expdata/
scp -i ~/.ssh/id_rsa.sakura -r $DIR/images/$1 root@133.242.130.178:/home/work/r/public/images/det/$1
# scp -i ~/.ssh/id_rsa.sakura -r $DIR/expdata/detail/11* root@133.242.130.178:/var/www/phpraw/expdata/detail/



# mongo, log
scp -i ~/.ssh/id_rsa.sakura $DIR/expdata/query/$1.txt root@133.242.130.178:
ssh -i ~/.ssh/id_rsa.sakura root@133.242.130.178 "mongo detect < ~/$1.txt"
