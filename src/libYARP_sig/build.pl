#! /usr/bin/perl
#
# compiling libYARP_sig
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#
#
#

use Getopt::Long;
use File::Copy;

print "Entering build process of YARP signal processing library...\n";
chomp ($tmp = `ver`);
if (index ($tmp, "Windows") < 0)
{
	die "This script is specific to Windows 2000/XP\n";
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
GetOptions ('debug' => sub { $debug = 1; $release = 0; },
            'release' => sub { $debug = 0; $release = 1; },
			'clean' => \$clean,
			'install' => \$install );

if ($clean)
{
	print "\nCleaning...\n";
	chdir "./src" or die "Cannot chdir to src: $!";

	call_msdev_and_print ("Debug", "CLEAN");
	call_msdev_and_print ("Release", "CLEAN");

	print "\n";
	chdir "../" or die "Cannot chdir to ..: $!";
}

if ($debug)
{
	print "\nCompiling debug\n";
	chdir "./src" or die "Cannot chdir to src: $!";

	call_msdev_and_print ("Debug", "BUILD");

	chdir "../" or die "Cannot chdir to ..: $!";
}

if ($release)
{
	print "\nCompiling optimized\n";
	chdir "./src" or die "Cannot chdir to src: $!";

	call_msdev_and_print ("Release", "BUILD");

	chdir ".." or die "Cannot chdir to ..: $!";
}

if ($install)
{
	print "\nInstalling YARP signal libraries to default install directory.\n";
	print "Later this might change to something smarter.\n";
	@my_headers = glob "./include/yarp/*.h";
	foreach $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or die "Can't copy .h files\n"; 
	}

	@my_libs = glob "./lib/winnt/*.lib";
	foreach $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/winnt/") or die "Can't copy any .lib file\n";
	}
}


sub call_msdev_and_print
{
	my ($version, $operation) = @_;

	open MSDEV, "msdev libYARP_sig.dsw /MAKE \"libYARP_sig - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}