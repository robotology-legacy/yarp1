#! /usr/bin/perl
#
# compiling libYARP_sig
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#		  --force to force the script to use the command line arguments
#
#		  --file <config_file>
#			where <config_file> is the filename of the context config file.
#
#

use Getopt::Long;
use File::Copy;
use Cwd;

print "Entering compile process of YARP signal processing libraries...\n";

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

require "$yarp_root/conf/configure.template.pl" or die "Can't find template file $yarp_root/conf/configure.template.pl\n";

check_os();

print "Ready to start...\n";

my $debug = '';
my $release = '';
my $clean = '';
my $install = '';
my $force = '';
my $config_file = "$yarp_root/conf/context.conf";
my %options = ();

GetOptions ('debug' => \$debug,
            'release' => \$release,
			'clean' => \$clean,
			'install' => \$install,
			'file=s' => \$config_file,
			'force' => \$force );

unless (-e $config_file)
{
	die "Can't find configuration file: $config_file\nPlease make sure a config file exists in \$YARP_ROOT/conf/\n";
}

load_config_file (\%options, $config_file);

my $os = $options{"Architecture<-OS"};

#
# override.
unless ($force)
{
	$debug = ($options{"Compile_Sig<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
	$release = ($options{"Compile_Sig<-Lib_Release"} eq "TRUE") ? 1 : $release;
	$install = ($options{"Compile_Sig<-Lib_Install"} eq "TRUE") ? 1 : $install;
	$clean = ($options{"Compile_Sig<-Lib_Clean"} eq "TRUE") ? 1 : $clean;
}

#
#
#
if ($clean)
{
	print "\nCleaning...\n";
	if ($os eq "winnt")
	{
		chdir "./src" or die "Cannot chdir to src: $!";

		call_msdev_and_print ("libYARP_sig", "Debug", "CLEAN");
		call_msdev_and_print ("libYARP_sig", "Release", "CLEAN");
	
		print "\n";
		chdir "../" or die "Cannot chdir to ..: $!";
	}
	elsif ($os eq "linux")
	{
                call_make_and_print ('', "clean");
	}
	elsif ($os eq "qnx6")
	{
                call_make_and_print ('', "clean");
	}	
}

if ($debug)
{
	print "\nCompiling debug\n";
	if ($os eq "winnt")
	{
		chdir "./src" or die "Cannot chdir to src: $!";
		call_msdev_and_print ("libYARP_sig", "Debug", "BUILD");
		chdir "../" or die "Cannot chdir to ..: $!";
	}
	elsif ($os eq "linux")
	{
                call_make_and_print ('', "CFAST=-g");
	}
	elsif ($os eq "qnx6")
	{
                call_make_and_print ('', "CFAST=-g");
	}
}

if ($release)
{
	print "\nCompiling optimized\n";
	if ($os eq "winnt")
	{
		chdir "./src" or die "Cannot chdir to src: $!";
		call_msdev_and_print ("libYARP_sig", "Release", "BUILD");
		chdir ".." or die "Cannot chdir to ..: $!";
	}
	elsif ($os eq "linux")
	{
                call_make_and_print ('', "CFAST=-O3");
	}
	elsif ($os eq "qnx6")
	{
                call_make_and_print ('', "CFAST=-O3");
	}
}

if ($install)
{
	print "\nInstalling YARP signal processing libraries to default install directory.\n";
	@my_headers = glob "./include/yarp/*.h";
	foreach $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy $file\n"; 
	}

	@my_libs = glob "./lib/$os/*.lib";
	foreach $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/$os/") or die "Can't copy $file\n";
	}
}

print "\nDone!\n";
