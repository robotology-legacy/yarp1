#! /bin/sh

#
#
#

yarp-connect /motorcmd/o /repeater/i
yarp-connect /repeater/o /armcontrol/behavior/i
yarp-connect /armcontrol/behavior/o /repeater/i
