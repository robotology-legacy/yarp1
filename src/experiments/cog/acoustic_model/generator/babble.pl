#!/usr/local/bin/perl

$babble_count = 100;
$babble_length = 5;

if ($#ARGV>=0)
{
    $babble_count = $ARGV[0];
}


open(VOCAB,"<vocab.pbsp");

@vocab = "";

while(<VOCAB>)
{
    chomp;
    split;
    if ($#_>=0)
    {
	$v = $_[0];
	if ($v=~/phn_/)
	{
	    push(@vocab,$v);
	}
    }
}

for ($i=0; $i<$babble_count; $i++)
{
    $len = int(2+rand($babble_length-1));
    for ($j=0; $j<$len; $j++)
    {
	$index = rand(int(1+$#vocab));
	print $vocab[$index], " ";
    }
    print "\n";
}










