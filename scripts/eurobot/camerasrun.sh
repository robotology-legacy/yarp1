#! /bin/sh

#
#
#

on -f morpheus framegrab -b 0 +left -l
on -f morpheus framegrab -b 1 +righr -l

camview +view001 -l -w 256 -h 256 
camview +view002 -l -w 256 -h 256 
