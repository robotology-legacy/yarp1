#! /usr/bin/perl
#
# compiling libYARP_OS tools
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

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT env var must be defined!\n";
}

require "$yarp_root/conf/configure.template.pl" or die "Can't find template file $yarp_root/conf/configure.template.pl\n";

my $exp_os = check_os();

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

if ($os ne $exp_os)
{
	die "The script has been called for $os but the environemnt report to be on $exp_os\n";
}

my @projects = qw/ yarp-connect yarp-read yarp-service yarp-write yarp-names /;

select STDERR;

if ($clean)
{
	print "\nCleaning...\n";
	if ($os eq "winnt")
	{
		foreach my $project (@projects)
		{
			chdir "./$project";
			call_msdev_and_print ("Debug", "CLEAN");
			call_msdev_and_print ("Release", "CLEAN");
			chdir "..";
		}
	}
	elsif ($os eq "linux" || $os eq "qnx6")
	{
		foreach my $project (@projects)
		{
			chdir "./$project";
			call_make_and_print ('', "clean");
			chdir "..";
		}
	}
	print "\n";
}

if ($debug)
{
	print "\nCompiling debug\n";
	if ($os eq "winnt")
	{
		foreach my $project (@projects)
		{
			chdir "./$project";
			call_msdev_and_print ("Debug", "BUILD");
			chdir "..";
		}
	}
	elsif ($os eq "linux" || $os eq "qnx6")
	{
		foreach my $project (@projects)
		{
			chdir "./$project";
			call_make_and_print ('', "CFAST=-g EXEC_TARGET=../bin/$os/$project");
			chdir "..";
		}
	}
}

if ($release)
{
	print "\nCompiling optimized\n";
	if ($os eq "winnt")
	{
		foreach my $project (@projects)
		{
			chdir "./$project";
			call_msdev_and_print ("Release", "BUILD");
			chdir "..";
		}
	}
	elsif ($os eq "linux" || $os eq "qnx6")
	{
		foreach my $project (@projects)
		{
			chdir "./$project";
			call_make_and_print ('', "CFAST=-O3 EXEC_TARGET=../bin/$os/$project");
			chdir "..";
		}
	}
}

if ($install)
{
	print "\nInstalling YARP_OS tools to default install directory.\n";

	foreach my $project (@projects)
	{
		foreach my $file (glob "./$project/obj/$os/*.exe")
		{
			print "Copying $file\n";
			copy "$file", "$yarp_root/bin/$os" or warn "Can't copy: $file\n";
		}
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
