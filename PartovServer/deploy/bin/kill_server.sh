#!/bin/bash
partovid=`ps aux | grep ./partov | grep -v grep | grep -v sudo | gawk '{ print $2 }'`
if [ $partovid ];
then
kill -9 $partovid
fi
