#! /usr/bin/perl
#
# compiling ACE dll (assuming 5.4.1).
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#
#		  --distribution is the path where ACE was unpacked.
#
# $Id: build.pl,v 1.2 2004-07-26 11:30:32 babybot Exp $
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

my $tmp;
my $yarp_root;

chomp ($tmp = `ver`);
if (index ($tmp, "Windows") < 0)
{
	die "This script is specific to Windows 2000/XP\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\n";
}

my $current_dir = getcwd;

my $debug = '';
my $release = '';
my $clean = '';
my $install = '';
my $distribution = undef;

GetOptions ('debug' => \$debug,
            'release' => \$release,
			'clean' => \$clean,
			'install' => \$install,
			'distribution=s' => \$distribution );

unless (defined $distribution)
{
	die "This script requires the parameter --distribution <path>\n";
}

#
# this is my personal trick to avoid buffering on output since
# I'm piping the scripts I'd like to get the output all on the same terminal.
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
		copy ($file, "$yarp_root/bin/winnt/") or die "Can't copy any .dll file\n";
	}

	@my_libs = glob "*.lib";
	foreach my $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/winnt/") or die "Can't copy any .lib file\n";
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
