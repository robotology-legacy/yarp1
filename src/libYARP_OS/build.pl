#! /usr/bin/perl
#
# compiling libYARP_OS
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

print "Entering compile process of YARP OS libraries...\n";

chomp ($tmp = `ver`);
if (index ($tmp, "Windows") < 0)
{
	print "This is a Windows 2000/XP specific script\n";
	print "Perhaps this procedure can be simply extended to\n"; 
	print "other OSes but for now, this is all experimental...\n";
	
	die "This script is specific to Windows 2000/XP\n";
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
#		print "Matched: $`<$&>$'\n";
		$contextual = $1;
	}
	elsif (/^([A-Za-z0-9_]+)= ?/)
	{
#		print "Matched: $`<$&>$'\n";
		$options{$contextual."<-".$1} = $';
	}
}

close CONFIG;

#print "dumping my hash table\n";
#while ( ($key, $value) = each %options )
#{
#	print "$key => $value\n";
#}

if ($options{"Compile_OS<-ACE_Rebuild"} eq "YES")
{
	if (exists $options{"Compile_OS<-ACE_PATH"})
	{
		print "Looking for ACE...\n";
		$options{"Compile_OS<-ACE_PATH"} =~ s/\$YARP_ROOT/$yarp_root/;
		if (-e $options{"Compile_OS<-ACE_PATH"} &&
			-e "$yarp_root/include/$options{\"Architecture<-OS\"}/ace" )
		{
			do_ace_compile ("$options{\"Compile_OS<-ACE_PATH\"}/build.pl --clean --debug --release --install --distribution $options{\"Compile_OS<-ACE_PATH\"}");
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
$debug = ($options{"Compile_OS<-Lib_Debug"} eq "TRUE") ? '1' : '';
$release = ($options{"Compile_OS<-Lib_Release"} eq "TRUE") ? '1' : '';
$install = ($options{"Compile_OS<-Lib_Install"} eq "TRUE") ? '1' : '';

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

	@my_libs = glob "./lib/winnt/*.lib";
	foreach $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/winnt/") or die "Can't copy any .lib file\n";
	}
}

if ($options{"Compile_OS<-Tools_Rebuild"} eq "YES")
{
	if ($options{"Compile_OS<-Tools_Debug"} eq "TRUE")
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_tools_compile ("build.pl --clean --debug --install");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
	else
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_tools_compile ("build.pl --clean --release --install");
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

	open MSDEV, "msdev libYARP_OS.dsw /MAKE \"libYARP_OS - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}




