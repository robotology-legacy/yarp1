#!/usr/bin/perl -w

use strict;

if ($#ARGV<0) {
    die "need the repository name";
}

my $rep = $ARGV[0];

my $prev = `test -e last/$rep && cat last/$rep`;
$prev =~ s/[\n\r]//g;

print STDERR "Working on $rep ; last timestamp=[$prev]\n";


system "mkdir -p last";

system "mkdir -p action";

open(HIST,"<history/$rep") || die "no history file";
open(ACTION,">action/$rep") || die "no action file";

my %mod;
my %del;

my $last = "";

my $on = 0;

if ($prev eq "") {
    $on = 1;
}

while (<HIST>) {
    if ($_ =~ /^([RAM]) ([^ ]+ [^ ]+ [^ ]+) [^ ]+ [^ ]+ ([^ ]+)[ \t]+([^ ]+)/) {
	my $act = $1;
	my $stamp = $2;
	my $file = $3;
	my $path = $4;
	#print STDERR "act [$act] file [$file] path [$path]\n";
	$last = $stamp;
	if ($on) {
	    my $name = "$path/$file";
	    if ($act =~ /[AM]/) {
		$mod{$name} = 1;
		if (defined($del{$name})) {
		    undef($del{$name});
		    print "a/m to an r file $name\n";
		}
	    } elsif ($act =~ /R/) {
		$del{$name} = 1;
		if (defined($mod{$name})) {
		    undef($mod{$name});
		    print "r to an a/m file $name\n";
		}
	    } else {
		die "unknown act";
	    }
	}
	if ($stamp eq $prev) {
	    $on = 1;
	}
    }
}


foreach my $name (keys %mod) {
    my $act = "mod $name\n";
    print $act;
    print ACTION $act;
}


foreach my $name (keys %del) {
    my $act = "del $name\n";
    print $act;
    print ACTION $act;
}


system "echo \"$last\" > last/$rep";

