#! /usr/bin/perl
#
# compiling libYARP_OS
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

print "Entering compile process of YARP OS libraries...\n";

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

open CONFIG, $config_file or die "Can't open config file $!";
print "Working with: $config_file\n";

my $contextual;
while (<CONFIG>)
{
	chomp;
	if (/^\[(\w+)\]\s?/)
	{
		$contextual = $1;
	}
	elsif (/^([A-Za-z0-9_\$]+)= ?/)
	{
		my $word = $1;
		if ($' =~ /([A-Za-z0-9_\/\$]+)\s?/)
		{
			$options{$contextual."<-".$word} = $1;
		}
	}
}

close CONFIG;

my $os = $options{"Architecture<-OS"};

if ($options{"Compile_OS<-ACE_Rebuild"} eq "YES")
{
	if (exists $options{"Compile_OS<-ACE_PATH"})
	{
		print "Looking for ACE...\n";
		$options{"Compile_OS<-ACE_PATH"} =~ s/\$YARP_ROOT/$yarp_root/;
		if (-e $options{"Compile_OS<-ACE_PATH"} &&
			-e "$yarp_root/include/$os/ace" )
		{
			do_ace_compile ("$options{\"Compile_OS<-ACE_PATH\"}/build.pl --clean --debug --release --install --distribution $options{\"Compile_OS<-ACE_PATH\"} --os $os");
		}
	}
	else
	{
		print "I'm assuming you've got ACE installed already!\n";
		print "Make sure this is the case to continue compilation\n";
	}
}

#
# override.
unless ($force)
{
	$debug = ($options{"Compile_OS<-Lib_Debug"} eq "TRUE") ? 1 : $debug;
	$release = ($options{"Compile_OS<-Lib_Release"} eq "TRUE") ? 1 : $release;
	$install = ($options{"Compile_OS<-Lib_Install"} eq "TRUE") ? 1 : $install;
	$clean = ($options{"Compile_OS<-Lib_Clean"} eq "TRUE") ? 1 : $clean;
}

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
	print "\nInstalling YARP libraries to default install directory.\n";
	print "Later this might change to something smarter.\n";
	@my_headers = glob "./include/yarp/*.h";
	foreach $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or die "Can't copy .h files\n"; 
	}

	@my_libs = glob "./lib/$os/*.lib";
	foreach $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/$os/") or die "Can't copy any .lib file\n";
	}
}

if ((!$force && $options{"Compile_OS<-Tools_Rebuild"} eq "YES") || 
	($force && $tools))
{
	if ($options{"Compile_OS<-Tools_Debug"} eq "TRUE" || (!$release && $force))
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_tools_compile ("build-os.pl --clean --debug --install --os $os");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
	else
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_tools_compile ("build-os.pl --clean --release --install --os $os");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
}
else
{
	print "You didn't ask to recompile the YARP tools\n";
}

#
#
#
sub do_tools_compile
{
	my ($exe) = @_;
	open TOOLS, "$exe|";
	while (<TOOLS>)
	{
		print;
	}
	close TOOLS;
}

sub do_ace_compile
{
	my ($exe) = @_;
	open ACE, "$exe|";
	while (<ACE>)
	{
		print;
	}
	close ACE;
}

sub call_msdev_and_print
{
	my ($version, $operation) = @_;

	open MSDEV, "msdev libYARP_OS.dsp /MAKE \"libYARP_OS - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}




