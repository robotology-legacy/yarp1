#!/usr/bin/perl -w

use strict;

if ($#ARGV<1) {
    die "need the repository names (from, to)";
}

my $src = $ARGV[0];
my $dest = $ARGV[1];

print STDERR "applying changes in $src to $dest\n";

open(ACTION,"<action/$src") || die "no action file";

while (<ACTION>) {
    if ($_ =~ /^([a-z]{3}) ([^ ]+)\/([^\/\n\r]+)/) {
	my $act = $1;
	my $path = $2;
	my $file = $3;
	print "act [$act] on [$path] [$file]\n";
    }
}

