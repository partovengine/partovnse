#!/bin/bash
perlid=`ps aux | grep perl | grep -v grep | gawk '{ print $2 }'`
partovid=`ps aux | grep ./partov | grep -v grep | gawk '{ print $2 }'`
kill -9 $perlid $partovid
