#! /usr/bin/perl
#
# compiling libYARP_math
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

print "Entering compile process of YARP math library...\n";

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

print "Working with: $config_file\n";
load_config_file (\%options, $config_file);

my $os = $options{"Architecture<-OS"};

#
# override.
unless ($force)
{
	$debug = ($options{"Compile_Matlab<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
	$release = ($options{"Compile_Matlab<-Lib_Release"} eq "TRUE") ? 1 : $release;
	$install = ($options{"Compile_Matlab<-Lib_Install"} eq "TRUE") ? 1 : $install;
	$clean = ($options{"Compile_Matlab<-Lib_Clean"} eq "TRUE") ? 1 : $clean;
}

#
#
#
if ($clean)
{
	print "\nCleaning...\n";
	chdir "./src" or die "Cannot chdir to src: $!";

	call_msdev_and_print ("port", "Debug", "CLEAN");
	call_msdev_and_print ("port", "Release", "CLEAN");
	
	print "\n";
	chdir "../" or die "Cannot chdir to ..: $!";
}

if ($debug)
{
	print "\nCompiling debug\n";
	chdir "./src" or die "Cannot chdir to src: $!";
	call_msdev_and_print ("port", "Debug", "BUILD");
	chdir "../" or die "Cannot chdir to ..: $!";
}

if ($release)
{
	print "\nCompiling optimized\n";
	chdir "./src" or die "Cannot chdir to src: $!";
	call_msdev_and_print ("port", "Release", "BUILD");
	chdir ".." or die "Cannot chdir to ..: $!";
}

if ($install)
{
	print "\nInstalling YARP math libraries to default install directory.\n";
	copy ("./src/port.dll", "$yarp_root/bin/$os/") or warn "Can't copy port.dll\n"; 
		
	@my_mfiles = glob "./support/*.m";
	foreach $file (@my_mfiles) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/bin/$os/") or warn "Can't copy $file\n"; 
	}

	@my_extra = glob "./logpolar_mapper/*.m";
	foreach $file (@my_extra)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/bin/$os/") or warn "Can't copy $file\n";
	}
}

print "\nDone!\n";
