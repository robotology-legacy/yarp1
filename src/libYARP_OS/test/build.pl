#! /usr/bin/perl
#
# compiling libYARP_OS tests
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#
#

use Getopt::Long;
use File::Copy;

print "Entering build process of YARP_OS tests...\n";
chomp ($ver = `ver`);
chomp ($uname = `uname`);

if (index ($ver, "Windows") < 0 && index ($uname, "CYGWIN") < 0)
{
	die "This script is specific to Windows 2000/XP or Cygwin\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT env var must be defined!\n";
}

my $debug = '';
my $release = '';
my $clean = '';
GetOptions ('debug' => sub { $debug = '1'; $release = '' },
            'release' => sub { $release = '1'; $debug = '' },
			'clean' => \$clean);

my @projects = qw/ demo00 demo01 /;

select STDERR;

if ($clean)
{
	print "\nCleaning...\n";
	foreach my $project (@projects)
	{
		call_msdev_and_print ($project, "Debug", "CLEAN");
		call_msdev_and_print ($project, "Release", "CLEAN");
	}
	print "\n";
}

if ($debug)
{
	print "\nCompiling debug\n";
	foreach my $project (@projects)
	{
		call_msdev_and_print ($project, "Debug", "BUILD");
	}
}

if ($release)
{
	print "\nCompiling optimized\n";
	foreach my $project (@projects)
	{
		call_msdev_and_print ($project, "Release", "BUILD");
	}
}

select STDOUT;

sub call_msdev_and_print
{
	my ($project, $version, $operation) = @_;

	open MSDEV, "msdev $project.dsp /MAKE \"$project - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}