#! /usr/bin/perl
#
# compiling libYARP_robot tools
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#		  --os <OS> the operating system you're building for
#
#

use Getopt::Long;
use File::Copy;

print "Entering build process of YARP_OS library tools...\n";
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
my $install = '';
my $os = '';
GetOptions ('debug' => sub { $debug = '1'; $release = '' },
            'release' => sub { $release = '1'; $debug = '' },
			'clean' => \$clean,
			'install' => \$install,
			'os=s' => \$os );

if ($os ne "winnt")
{
	die "This script is not yet tuned for OSes apart \"winnt\"\n";
}

my @projects = qw/ grabber grabber-logpolar /; # camview-winnt /;

select STDERR;

if ($clean)
{
	print "\nCleaning...\n";
	foreach my $project (@projects)
	{
		chdir "./$project";
		call_msdev_and_print ("Debug", "CLEAN");
		call_msdev_and_print ("Release", "CLEAN");
		chdir "..";
	}
	print "\n";
}

if ($debug)
{
	print "\nCompiling debug\n";
	foreach my $project (@projects)
	{
		chdir "./$project";
		call_msdev_and_print ("Debug", "BUILD");
		chdir "..";
	}
}

if ($release)
{
	print "\nCompiling optimized\n";
	foreach my $project (@projects)
	{
		chdir "./$project";
		call_msdev_and_print ("Release", "BUILD");
		chdir "..";
	}
}

if ($install)
{
	print "\nInstalling YARP_robot tools to default install directory.\n";

	foreach my $project (@projects)
	{
		print "Copying ./$project/obj/$project.exe\n";
		copy "./$project/obj/$project.exe", "$yarp_root/bin/$os";
	}
}

select STDOUT;

sub call_msdev_and_print
{
	my ($version, $operation) = @_;
	my @dsps = glob "*.dsp";

	$dsps[0] =~ /.dsp/;

	open MSDEV, "msdev $dsps[0] /MAKE \"$` - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}