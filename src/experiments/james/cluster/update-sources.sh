#!/bin/bash

SRC="/etc/apt/sources.list"

grep venus $SRC || (
    echo Adding local package source
    sudo sh -c "echo 'deb http://venus.lira.dist.unige.it/deb package/' >> $SRC"
)
