#! /usr/bin/perl
#
# compiling ACE dll (assuming 5.4.1).
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#
#		  --distribution <PATH> is the path where ACE was unpacked
#		  --os <OS> is the operating system you're compiling for
#
# $Id: build.pl,v 1.3 2004-07-26 15:04:29 babybot Exp $
#
# This script can be (at least in theory) configured to
# do some useful thing in Linux and/or Qnx too. It's definitely
# useful on Windows which lacks a smart enough make utility
# not to mention bat files!
#
#

use Getopt::Long;
use File::Copy;
use Cwd;

use File::Find qw(find);
use File::Path qw(mkpath);
use File::Copy qw(copy);
use strict;

print "Entering build process for ACE libraries...\n";
print "Assuming you've unpacked version 5.4.1, it might not work on other releases\n";
print "since Windows project names tend not to be uniform across releases\n";

my $ver;
my $uname;
my $yarp_root;

chomp ($ver = `ver`);
chomp ($uname = `uname`);
if (index ($ver, "Windows") < 0 && index ($uname, "CYGWIN") < 0)
{
	die "This script is specific to Windows 2000/XP or Cygwin\nCompilation will always happen for winnt\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "\$YARP_ROOT environment variable must be defined!\n";
}

my $current_dir = getcwd;

my $debug = '';
my $release = '';
my $clean = '';
my $install = '';
my $distribution = undef;
my $os = undef;

GetOptions ('debug' => \$debug,
            'release' => \$release,
			'clean' => \$clean,
			'install' => \$install,
			'distribution=s' => \$distribution,
			'os=s' => \$os );

unless (defined $distribution)
{
	die "This script requires the parameter --distribution <path>\n";
}

if ($os ne "winnt")
{
	die "This script is not yet tuned for OSes apart \"winnt\"\n";
}

#
# this is my personal trick to avoid buffering on output since
# I'm piping the scripts I'd like to get the output all on the same terminal
# as soon as it gets produced by the script/executable.
#
select STDERR;

if ($clean)
{
	print "\nCleaning...\n";
	chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";

	call_msdev_and_print ("Debug", "CLEAN");
	call_msdev_and_print ("Release", "CLEAN");
	
	print "\n";
	chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
}

if ($debug)
{
	print "\nCompiling debug\n";

	copy ("$yarp_root/include/winnt/ace/config.h", "$distribution/ace/") or die "Can't copy config.h file\n"; 
	chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";

	call_msdev_and_print ("Debug", "BUILD");
	
	unlink "config.h";
	chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
}

if ($release)
{
	print "\nCompiling release\n";

	copy ("$yarp_root/include/winnt/ace/config.h", "$distribution/ace/") or die "Can't copy config.h file\n"; 
	chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";

	call_msdev_and_print ("Release", "BUILD");
	
	unlink "config.h";
	chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
}

if ($install)
{
	print "\nInstalling ACE libraries to default install directory.\n";
	print "Later this might change to something smarter.\n";

	chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
	
	my @my_files = glob "*.h *.inl *.cpp *.i";
	foreach my $file (@my_files) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/ace/") or die "Can't copy .h files\n"; 
	}

	recursive_copy ("./os_include", "$yarp_root/include/ace/os_include");

	my @my_libs = glob "../bin/*.dll";
	foreach my $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/bin/$os/") or die "Can't copy any .dll file\n";
	}

	@my_libs = glob "*.lib";
	foreach my $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/$os/") or die "Can't copy any .lib file\n";
	}

	chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
}

select STDOUT;

#
#
#
sub call_msdev_and_print
{
	my ($version, $operation) = @_;

	open MSDEV, "msdev ace.dsw /MAKE \"ACE DLL - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}

#
#
#
#

my $wanted_target = undef;

sub recursive_copy
{
	my ($source, $target) = @_;
	my $cur_dir = getcwd;

    chdir($source)
        or die("Unable to change directory to \"$source\": \l$!.\n");

	$wanted_target = $target;
    find(\&wanted, ".");

	chdir($cur_dir)
		or die("Can't chdir to my current directory \"$cur_dir\": $!\n");
}

sub wanted 
{
    my $target_file = "$wanted_target/$File::Find::name";

    if (-d) 
	{
        mkpath($target_file) or
            warn("Unable to make path \"$target_file\".\n");
    } 
	else 
	{
		print "Copying $File::Find::name\n";
        unless (copy($_, "$wanted_target/$File::Find::name")) 
		{
            warn(
                "Unable to copy \"$File::Find::name\" to ",
                "\"$target_file\": \l$!.\n"
            );
        }
    }
}
