#!/bin/bash
umask 0022
ulimit -c unlimited
ulimit -n 10000
nice ./partov 1>>logn.txt 2>>loge.txt
