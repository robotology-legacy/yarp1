#! /bin/sh

#
#
#
yarp-connect /repeater/o !/armrandom/behavior/i
yarp-connect /armrandom/behavior/o !/repeater/i
