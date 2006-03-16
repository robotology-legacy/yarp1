#!/usr/bin/perl -w

use strict;

my $fake = 0;
my $change = 0;

sub loadCvs {
    my $f = shift;
    my @line = ();
    open(FIN,"<$f");
    while (<FIN>) {
	chomp($_);
	if (!($_ =~ /\$Id: apply.pl,v 1.2 2006-03-16 10:37:14 eshuy Exp $/)) {
	    push(@line,$_);
	}
    }
    close(FIN);
    return join("\n",@line);
}

sub compare {
    my $f1 = shift;
    my $f2 = shift;
    my $txt1 = loadCvs($f1);
    my $txt2 = loadCvs($f2);
    return ($txt1 eq $txt2);
}

sub ensureDirectory {
    my $dest = shift;
    my $path = shift;
    my @parts = split('/',$path);
    my $base = "state/$dest";
    for (my $i=0; $i<=$#parts; $i++) {
	my $next = $base . "/" . $parts[$i];
	if (!(-e $next)) {
	    print "Making directory $next\n";
	    system "mkdir -p $next";
	}
	if (!(-e "$next/CVS")) {
	    if (!$fake) {
		system "cd $base; cvs add $parts[$i]";
	    }
	    $change++;
	}
	$base = $next;
    }
}

sub applyMod {
    my $src = shift;
    my $dest = shift;
    my $path = shift;
    my $file = shift;
    my $srcName = "state/$src/$path/$file";
    if (-e $srcName) {
	ensureDirectory($dest,$path);
	my $destName = "state/$dest/$path/$file";
	my $add = (!(-e $destName));
	my $diff = 0;
	if ($add) {
	    if (!$fake) {
		system "cp $srcName $destName";
	    }
	    $diff = 1;
	} else {
	    my $eq = compare($srcName,$destName);
	    if (!$eq) {
		if (!$fake) {
		    system "cp $srcName $destName";
		}
		$diff = 1;
	    }
	}
	if ($diff) {
	    print "Brought in $destName from $srcName\n";
	    $change++;
	}
	if ($add) {
	    if (!$fake) {
		system "cd state/$dest/$path; cvs add $file";
	    }
	}
    } else {
	print STDERR "could not find $srcName\n";
    }
}

sub applyRem {
    my $src = shift;
    my $dest = shift;
    my $path = shift;
    my $file = shift;
    my $destName = "state/$dest/$path/$file";
    if (-e $destName) {
	system "rm $destName";
	$change++;
	if (!$fake) {
	    system "cd state/$dest/$path; cvs remove $file";
	}
	print "Removed $destName\n";
    } 
}

if ($#ARGV<1) {
    die "need the repository names (from, to)";
}

my $src = $ARGV[0];
my $dest = $ARGV[1];

print STDERR "applying changes in $src to $dest\n";

open(ACTION,"<action/$src") || die "no action file";

my $srcRoot = `cat state/$src/CVS/Repository`;
chomp($srcRoot);
my $qSrcRoot = quotemeta($srcRoot);
my $destRoot = `cat state/$dest/CVS/Repository`;
chomp($destRoot);
my $qDestRoot = quotemeta($destRoot);

while (<ACTION>) {
    if ($_ =~ /^([a-z]{3}) ([^ ]+)\/([^\/\n\r]+)/) {
	my $act = $1;
	my $path = $2;
	my $file = $3;
	#print "source root is $srcRoot\n";
	$path =~ s/^$qSrcRoot\/?//;
	print "act [$act] on [$path] [$file]\n";
	if ($act eq "mod") {
	    #$path =~ s/^[^\/]*\///;
	    applyMod($src,$dest,$path,$file);
	} elsif ($act eq "rem") {
	    applyRem($src,$dest,$path,$file);
	}
    }
}

if ($change>0) {
    if (!$fake) {
	system "cd state/$dest; cvs commit -m \"automatic patch from $src team\"";
    }
}

print "Change count: $change\n";

if ($fake) {
    print "FAKE flag was on so no commits done\n";
}

