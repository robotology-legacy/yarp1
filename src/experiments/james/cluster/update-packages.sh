#!/bin/bash

# this will stop and ask if you want to install packages
# add a "yes | " at the start I guess if you don't want this

sudo apt-get update
#sudo apt-get remove libyarp-os-dev
sudo apt-get install libyarp-sig-compat-dev libyarp-os-compat-dev libyarp-sig-dev libyarp-os-dev

