#!/usr/bin/perl -w

use strict;

use Cwd;
use File::Copy;
use File::Find;
use File::Path;

my $name = "NOPACKAGE";
my $root = ".";



my @incvs = ();

sub ingest {
    my $x = shift;
    my $pre = shift;
    if ($pre =~ /CVS$/) {
	$pre =~ s/CVS$//;
	#print "ingesting $x\n";
	open(FIN,"<$x") || die "cannot open: $x\n";
	while (<FIN>) {
	    chomp($_);
	    my @attr = split(/\//,$_);
	    if ($#attr>=1) {
		my $fname = $attr[1];
		if ($#attr>=2) {
		    my $ver = $attr[2];
		    my $f = "$pre$fname";
#		    print "[$f] is in cvs\n";
		    push(@incvs,$f);
		}
	    }
	}
	close(FIN);
    }
}

sub show {
    my $s = $_;
    my $x = $File::Find::name;
    my $d = $File::Find::dir;
    if ($s eq "Entries") {
	#print ">>> directory $d file $s\n";
	ingest($s, $d);
    }
}

sub getInCVS {
    my $dir = shift;
    @incvs = ();
    find(\&show,($dir));
    return @incvs;
}

while (<>) {
    chomp($_);
    $_ =~ s/\#.*//;
    print "command: $_\n";
    my @args = split(/[ \t]+/, $_);
    if ($#args>=0) {
	my $cmd = $args[0];
	if ($cmd eq "package") {
	    $name = $args[1];
#	    print "Creating package [$name]\n";
	    mkpath($name);
	} elsif ($cmd eq "copy") {
#	my $target = $name . "/" . $args[1];
#	my $source = $root . "/" . $args[1];
	    my $mode = $args[1];
	    my $source = $args[2];
#	    print "source [$source]\n";
	    my $cwd = cwd();
	    chdir($root);
#	    print "In directory ", cwd(), "\n";
	    my @inventory;
	    if ($mode =~ /file/i) {
		@inventory = ($source);
	    } else {
		@inventory = getInCVS($source);
	    }
	    chdir($cwd);
	    foreach my $f (@inventory) {
		my $f1 = $root . '/' . $f;
		my $f2 = $cwd . '/' . $name . '/' . $f;
		#print ">>> copy $f1 $f2\n";
		my $dir = $f2;
		$dir =~ s:/[^/]*$::;
		#print ">>> copy $f1 $f2 (dir $dir)\n";
		mkpath($dir);
		copy($f1,$f2) or die "Copy failed ($f1 to $f2): $!";
	    }
#	mkpath($target,1);
#	print "Source is $source\n";
#	my $cwd = cwd();
#	chdir($root);
#	find(\&show,($source));
#	chdir($cwd);
	} elsif ($cmd eq "root") {
	    $root = $args[1];
	    #print "Setting root directory to [$root]\n";
	}
    }
}

