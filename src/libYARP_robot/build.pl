#! /usr/bin/perl
#
# compiling libYARP_robot
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#
#		  --file <config_file>
#			where <config_file> is the filename of the context config file.
#
#

use Getopt::Long;
use File::Copy;
use Cwd;

print "Entering compile process of YARP_robot libraries...\n";

chomp ($ver = `ver`);
chomp ($uname = `uname`);
if (index ($ver, "Windows") < 0 && index ($uname, "CYGWIN") < 0)
{
	print "This is a Windows 2000/XP specific script\n";
	print "Perhaps this procedure can be simply extended to\n"; 
	print "other OSes but for now, this is all experimental...\n";
	
	die "This script is specific to Windows 2000/XP or Cygwin\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

print "Ready to start...\n";

my $debug = '';
my $release = '';
my $clean = '';
my $install = '';
my $config_file = "$yarp_root/conf/context.conf";
my %options = ();

GetOptions ('debug' => \$debug,
            'release' => \$release,
			'clean' => \$clean,
			'install' => \$install,
			'file=s' => \$config_file );

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
		$contextual = $1;
	}
	elsif (/^([A-Za-z0-9_]+)= ?/)
	{
		$options{$contextual."<-".$1} = $';
	}
}

close CONFIG;

my $os = "$options{\"Architecture<-OS\"}";
my $context_dir = "$options{\"Architecture<-Hardware_Name\"}_$os";
my $project_name = "libYARP_robot_$options{\"Architecture<-Hardware_Name\"}";

#
# override.
$debug = ($options{"Compile_Robot<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
$release = ($options{"Compile_Robot<-Lib_Release"} eq "TRUE") ? 1 : $release;
$install = ($options{"Compile_Robot<-Lib_Install"} eq "TRUE") ? 1 : $install;
$clean = ($options{"Compile_Robot<-Lib_Clean"} eq "TRUE") ? 1 : $clean;

#
#
#
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
	print "\nInstalling YARP_robot libraries to default install directory.\n";

	@my_headers = glob "./include/yarp/*.h ./$options{\"Architecture<-Hardware_Name\"}/yarp/*.h";
	foreach my $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy .h files\n"; 
	}

	copy ("./lib/$os/$project_name.lib", "$yarp_root/lib/$os/libYARP_robot.lib") or warn "Can't copy \"./lib/$os/libYARP_robot.lib\"\n";
	copy ("./lib/$os/$project_name"."d.lib", "$yarp_root/lib/$os/libYARP_robotd.lib") or warn "Can't copy \"./lib/$os/libYARP_robotd.lib\"\n";
	print "\nLibraries installed in $yarp_root/lib/$os\n";
}

print "\nDone!\n";

sub call_msdev_and_print
{
	my ($version, $operation) = @_;

	open MSDEV, "msdev $project_name.dsp /MAKE \"$project_name - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}




