#! /bin/sh

#
#
#

yarp-connect /motorcmd/o !/repeater/i
yarp-connect /repeater/o !/headcontrol/behavior/i
yarp-connect /headcontrol/behavior/o !/repeater/i
