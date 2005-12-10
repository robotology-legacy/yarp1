#!/usr/bin/perl -w

use strict;

use XML::Simple;

sub get {
    my $hh = shift;
    my %h = %$hh;
    my $name = shift;
    return ${$h{$name}}{value};
}

my $xml = XMLin('monet.xml');
my $phoneList = ${$xml}{MainPhoneList};
my %phones = %{$$phoneList{Phone}};

my @buttons = ();

foreach my $name (keys %phones) {
    my $phone = $phones{$name};
    my $p = $$phone{Parameter};

    my $glotVol = get($p,"glotVol");
    my $aspVol = get($p,"aspVol");
    my $velum = get($p,"velum");
    my $fricVol = get($p,"fricVol");
    my $fricPos = get($p,"fricPos");
    my $fricCF = get($p,"fricCF")/100;
    my $fricBW = get($p,"fricBW")/100;
    my @rs = ();
    for (my $i=1; $i<=8; $i++) {
	push(@rs,get($p,"r$i"));
    }
    if ($name =~ /^[a-z]+$/) {
	print STDERR "$name: $velum\n";
	my $bname = ".push_$name";
	print "button $bname -text \"$name\" -width 5 -command {\n";
	print "\ttube ", join(" ",@rs), " $velum\n";
	print "\tfric $fricVol $fricPos $fricCF $fricBW\n";
	print "\nsnd $glotVol $aspVol\n";
	print "}\n\n";
	push(@buttons,$bname);
    }
}

my $len = $#buttons+1;
my $w = 3;
my $h = int(0.5+$len/5);
my $r = 1;
my $c = 1;
foreach my $name (sort @buttons) {
#    print "pack $name -side right\n";
    print "grid $name -in .phone -row $r -column $c\n";
    $c++;
    if ($c>$w) {
	$c = 1;
	$r++;
    }
}
print "pack .phone -side right -fill x\n";

