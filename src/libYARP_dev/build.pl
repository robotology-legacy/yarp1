#! /usr/bin/perl
#
# compiling libYARP_dev
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

print "Entering compile process of YARP_dev libraries...\n";

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

my $os = "$options{\"Architecture<-OS\"}";
my $context_dir = "$options{\"Architecture<-Hardware_Name\"}_$os";
my $project_name = "libYARP_dev_$options{\"Architecture<-Hardware_Name\"}";

#
# override.
unless ($force)
{
	$debug = ($options{"Compile_Dev<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
	$release = ($options{"Compile_Dev<-Lib_Release"} eq "TRUE") ? 1 : $release;
	$install = ($options{"Compile_Dev<-Lib_Install"} eq "TRUE") ? 1 : $install;
	$clean = ($options{"Compile_Dev<-Lib_Clean"} eq "TRUE") ? 1 : $clean;
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
	print "\nInstalling YARP_dev libraries to default install directory.\n";

	@my_headers = glob "./include/yarp/*.h";
	foreach my $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy $file\n"; 
	}

	move ("./lib/$os/$project_name"."d_x.lib", "./lib/$os/libYARP_devd_x.lib") or warn "Can't move \"./lib/$os/$project_name"."d_x.lib\"\n";
	move ("./lib/$os/$project_name"."_x.lib", "./lib/$os/libYARP_dev_x.lib") or warn "Can't move \"./lib/$os/$project_name"."_x.lib\"\n";

	$libraries = '';
	foreach my $device (glob "*")
	{
		if (-d "$device/$os/yarp" && $options{"Compile_Dev<-DD_$device"} eq "YES")
		{
			foreach my $file (glob "./$device/$os/yarp/*.h")
			{
				copy($file, "$yarp_root/include/yarp/") or warn "Can't copy $file\n";
			}

			foreach my $file (glob "./$device/$os/dd_orig/lib/*.lib")
			{
				$file =~ s#/#\\#g;
				$libraries = "$libraries $file";
			}

			foreach my $file (glob "./$device/$os/dd_orig/bin/*.dll")
			{
				copy($file, "$yarp_root/bin/$os/") or warn "Can't copy $file\n";
			}
		}
	}

	if ($libraries ne '')
	{
		print "\nNow building libraries...\n";

		$libraries =~ s# #\"#;
		$libraries =~ s# #\" \"#g;
		$libraries = "$libraries"."\"";
		
		print `lib \".\\lib\\$os\\libYARP_dev_x.lib\" $libraries /out:\".\\lib\\$os\\libYARP_dev.lib\"`;
		print `lib \".\\lib\\$os\\libYARP_devd_x.lib\" $libraries /out:\".\\lib\\$os\\libYARP_devd.lib\"`;
	}
	else
	{
		copy ("./lib/$os/libYARP_devd_x.lib", "./lib/$os/libYARP_devd.lib") or warn "Can't copy \"./lib/$os/libYARP_devd_x.lib\"\n";
		copy ("./lib/$os/libYARP_dev_x.lib", "./lib/$os/libYARP_dev.lib") or warn "Can't copy \"./lib/$os/libYARP_dev_x.lib\"\n";
	}

	copy ("./lib/$os/libYARP_dev.lib", "$yarp_root/lib/$os/") or warn "Can't copy \"./lib/$os/libYARP_dev.lib\"\n";
	copy ("./lib/$os/libYARP_devd.lib", "$yarp_root/lib/$os/") or warn "Can't copy \"./lib/$os/libYARP_devd.lib\"\n";

	print "\nLibraries installed in $yarp_root/lib/$os\n";
}

print "\nDone!\n";
