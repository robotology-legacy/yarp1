#!/usr/bin/perl -w

use strict;

# assume processing output of "cvs update -d"

while (<>) {
    my $act = $_;
    chomp($act);
    if ($act =~ /^([^ ]) ([^ ].*)$/) {
	my $key = $1;
	my $name = $2;
	#print "key [$key] file [$name]\n";
	if ($key eq "U" || $key eq "P" || $key eq "M" || $key eq "C") {
	    print "+ $name\n";
	}
	if ($key eq "?") {
	    print "- $name\n";
	}
    }
}



