#! /usr/bin/perl
#
# compiling libYARP_dev
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

print "Entering compile process of YARP_dev libraries...\n";

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
my $project_name = "libYARP_dev_$options{\"Architecture<-Hardware_Name\"}";

#
# override.
$debug = ($options{"Compile_Dev<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
$release = ($options{"Compile_Dev<-Lib_Release"} eq "TRUE") ? 1 : $release;
$install = ($options{"Compile_Dev<-Lib_Install"} eq "TRUE") ? 1 : $install;
$clean = ($options{"Compile_Dev<-Lib_Clean"} eq "TRUE") ? 1 : $clean;

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
	print "\nInstalling YARP_dev libraries to default install directory.\n";

	@my_headers = glob "./include/yarp/*.h";
	foreach my $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy .h files\n"; 
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
				copy($file, "$yarp_root/include/yarp/") or warn "Can't copy .h files\n";
			}

			foreach my $file (glob "./$device/$os/dd_orig/lib/*.lib")
			{
				$file =~ s#/#\\#g;
				$libraries = "$libraries $file";
			}

			foreach my $file (glob "./$device/$os/dd_orig/bin/*.dll")
			{
				copy($file, "$yarp_root/bin/$os/") or warn "Can't copy .dll files\n";
			}
		}
	}

	print "\nNow building libraries...\n";

	print `lib .\\lib\\$os\\libYARP_dev_x.lib $libraries /out:.\\lib\\$os\\libYARP_dev.lib`;
	print `lib .\\lib\\$os\\libYARP_devd_x.lib $libraries /out:.\\lib\\$os\\libYARP_devd.lib`;

	copy ("./lib/$os/libYARP_dev.lib", "$yarp_root/lib/$os/") or warn "Can't copy \"./lib/$os/libYARP_dev.lib\"\n";
	copy ("./lib/$os/libYARP_devd.lib", "$yarp_root/lib/$os/") or warn "Can't copy \"./lib/$os/libYARP_devd.lib\"\n";
	print "\nLibraries installed in $yarp_root/lib/$os\n";
}

print "\nDone!\n";

#if ($options{"Compile_OS<-Tools_Rebuild"} eq "YES")
#{
#	if ($options{"Compile_OS<-Tools_Debug"} eq "TRUE")
#	{
#		my $current_dir = getcwd;
#		chdir "../tools/" or die "Can't chdir to tools directory\n";
#		do_tools_compile ("build.pl --clean --debug --install --os $os");
#		chdir $current_dir or die "Can't chdir to $current_dir\n";
#	}
#	else
#	{
#		my $current_dir = getcwd;
#		chdir "../tools/" or die "Can't chdir to tools directory\n";
#		do_tools_compile ("build.pl --clean --release --install --os $os");
#		chdir $current_dir or die "Can't chdir to $current_dir\n";
#	}
#}
#else
#{
#	print "You didn't ask to recompile the YARP tools\n";
#}
#
#
#
#
#
#sub do_tools_compile
#{
#	my ($exe) = @_;
#	open TOOLS, "$exe|";
#	while (<TOOLS>)
#	{
#		print;
#	}
#	close TOOLS;
#}

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




