#! /usr/bin/perl
#
#	--file <config_file>
#		where <config_file> is the filename of the context config file.
#

use Getopt::Long;
use File::Copy;

print "Entering configure process of YARP environment...\n";

chomp ($tmp = `ver`);
if (index ($tmp, "Windows") < 0)
{
	print "This is a Windows 2000/XP specific script\n";
	print "Perhaps this procedure can be simply extended to\n"; 
	print "other OSes but for now, this is all experimental...\n";
	
	die "This script is specific to Windows 2000/XP\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

print "Ready to start...\n";

my $config_file = "$yarp_root/conf/context.conf";
my %options = ();

GetOptions ('file=s' => \$config_file );

unless (-e $config_file)
{
	die "Can't find configuration file: $config_file\nPlease make sure a config file exists in \$YARP_ROOT/conf/\n";
}

open CONFIG, $config_file or die "Can't open config file $!";
print "Working with: $config_file\n";

my $contextual;
while (<CONFIG>)
{
	chomp;
	if (/^\[(\w+)\]$/)
	{
#		print "Matched: $`<$&>$'\n";
		$contextual = $1;
	}
	elsif (/^([A-Z0-9_]+)= ?/)
	{
		$options{$contextual."_".$1} = $';
	}
}

close CONFIG;

#print "dumping my hash table\n";
#while ( ($key, $value) = each %options )
#{
#	print "$key => $value\n";
#}

print "Starting the configuration procedure\n";

if (exists $options{"Compile_OS_ACE_PATH"})
{
	print "Looking for ACE...\n";
	$options{"Compile_OS_ACE_PATH"} =~ s/\$YARP_ROOT/$yarp_root/;
	if (-e $options{"Compile_OS_ACE_PATH"} &&
		-e "$yarp_root/include/$options{\"Architecture_OS\"}/ace" )
	{
		print "Compiling ACE...\n";
		do_ace_compile("$yarp_root/include/$options{\"Architecture_OS\"}/ace/build.pl --clean --debug --release --install --distribution $options{\"Compile_OS_ACE_PATH\"}");
	}
}
else
{
	print "I'm assuming you've got ACE installed already!\n";
	print "Make sure this is the case to continue the configuration\n";
}


sub do_ace_compile
{
	my ($exe) = @_;
	open ACE, "$exe|";
	while (<ACE>)
	{
		print;
	}
	close ACE;
}


sub call_msdev_and_print
{
	my ($version, $operation) = @_;

	open MSDEV, "msdev libYARP_OS.dsw /MAKE \"libYARP_OS - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}