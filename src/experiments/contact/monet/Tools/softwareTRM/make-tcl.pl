#!/usr/bin/perl -w

use strict;

my $out = "control.tcl";

system "cp control-base.tcl $out";

system "./process-xml.pl >> $out";

system "cat control-tail.tcl >> $out";


