#! /usr/bin/perl
#
# compiling libYARP_robot
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#		  --force to force the script to use the command line arguments
#		  --tools to compile the tools
#
#		  --file <config_file>
#			where <config_file> is the filename of the context config file.
#
#

use Getopt::Long;
use File::Copy;
use Cwd;

print "Entering compile process of YARP_robot libraries...\n";

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
my $tools = '';
my $config_file = "$yarp_root/conf/context.conf";
my %options = ();

GetOptions ('debug' => \$debug,
            'release' => \$release,
			'clean' => \$clean,
			'install' => \$install,
			'file=s' => \$config_file,
			'force' => \$force,
			'tools' => \$tools );

unless (-e $config_file)
{
	die "Can't find configuration file: $config_file\nPlease make sure a config file exists in \$YARP_ROOT/conf/\n";
}

print "Working with: $config_file\n";
load_config_file (\%options, $config_file);


my $os = "$options{\"Architecture<-OS\"}";
my $context_dir = "$options{\"Architecture<-Hardware_Name\"}_$os";
my $project_name = "libYARP_robot_$options{\"Architecture<-Hardware_Name\"}";

#
# override.
unless ($force)
{
	$debug = ($options{"Compile_Robot<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
	$release = ($options{"Compile_Robot<-Lib_Release"} eq "TRUE") ? 1 : $release;
	$install = ($options{"Compile_Robot<-Lib_Install"} eq "TRUE") ? 1 : $install;
	$clean = ($options{"Compile_Robot<-Lib_Clean"} eq "TRUE") ? 1 : $clean;
}

#
#
#
if ($clean)
{
	print "\nCleaning...\n";
	chdir "./src" or die "Cannot chdir to src: $!";

	call_msdev_and_print ($project_name, "Debug", "CLEAN");
	call_msdev_and_print ($project_name, "Release", "CLEAN");
	
	print "\n";
	chdir "../" or die "Cannot chdir to ..: $!";
}

if ($debug)
{
	print "\nCompiling debug\n";
	chdir "./src" or die "Cannot chdir to src: $!";
	call_msdev_and_print ($project_name, "Debug", "BUILD");
	chdir "../" or die "Cannot chdir to ..: $!";
}

if ($release)
{
	print "\nCompiling optimized\n";
	chdir "./src" or die "Cannot chdir to src: $!";
	call_msdev_and_print ($project_name, "Release", "BUILD");
	chdir ".." or die "Cannot chdir to ..: $!";
}

if ($install)
{
	print "\nInstalling YARP_robot libraries to default install directory.\n";

	@my_headers = glob "./include/yarp/*.h ./$options{\"Architecture<-Hardware_Name\"}/yarp/*.h";
	foreach my $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy $file\n"; 
	}

	copy ("./lib/$os/$project_name.lib", "$yarp_root/lib/$os/libYARP_robot.lib") or warn "Can't copy \"./lib/$os/libYARP_robot.lib\"\n";
	copy ("./lib/$os/$project_name"."d.lib", "$yarp_root/lib/$os/libYARP_robotd.lib") or warn "Can't copy \"./lib/$os/libYARP_robotd.lib\"\n";
	print "\nLibraries installed in $yarp_root/lib/$os\n";
}


#
# tools compile.
#
if ((!$force && $options{"Compile_Robot<-Tools_Rebuild"} eq "YES") ||
	($force && $tools))
{
	if ($options{"Compile_Robot<-Tools_Debug"} eq "TRUE" || (!$release && $force))
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_ext_compile ("build-robot.pl --clean --debug --install --os $os");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
	else
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_ext_compile ("build-robot.pl --clean --release --install --os $os");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
}
else
{
	print "You didn't ask to recompile the YARP robot tools\n";
}

print "\nDone!\n";


