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
# $Id: build.pl,v 1.13 2006-01-26 23:08:32 gmetta Exp $
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

my $ver;
my $uname;
my $yarp_root;

chomp ($ver = `ver`);
chomp ($uname = `uname`);
if (index ($ver, "Windows") < 0 && index ($uname, "CYGWIN") < 0
	&& index ($uname, "QNX") < 0 && index ($uname, "Linux") < 0
	&& index ($uname, "Darwin") < 0)
{
	die "This script is specific to Windows 2000/XP, Cygwin, Linux, Darwin or Qnx version 6\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "\$YARP_ROOT environment variable must be defined!\n";
}

#require "$yarp_root/conf/configure.template.pl" or die "Can't find template file $yarp_root/conf/configure.template.pl\n";

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

if ($os ne "winnt" && $os ne "qnx6" && $os ne "linux" && $os ne "darwin")
{
	die "This script is not yet tuned for OSes apart \"winnt\", \"linux\", \"darwin\" and \"qnx6\"\n";
}

#
# this is my personal trick to avoid buffering on output since
# I'm piping the scripts I'd like to get the output all on the same terminal
# as soon as it gets produced by the script/executable.
#
select STDERR;

my $project_name = '';
my $file_name = '';

if ($os eq "winnt")
{
	print "I'm trying to guess the project file name\n";
	search_project();
}

if ($clean)
{
	print "\nCleaning...\n";
	chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";

	if ($os eq "winnt")
	{
		call_msdev_and_print ("Debug", "CLEAN", $project_name, $file_name);
		call_msdev_and_print ("Release", "CLEAN", $project_name, $file_name);
	}
	elsif ($os eq "qnx6")
	{
		symlink ("$yarp_root/include/$os/ace/platform_qnx_rtp_gcc.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
		symlink ("$yarp_root/include/$os/ace/config-qnx-rtp-62x.h", "$distribution/ace/config.h");
		call_make_and_print ('', 'clean');
	}
	elsif ($os eq "linux")
	{
		symlink ("$yarp_root/include/$os/ace/platform_linux.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
		symlink ("$yarp_root/include/$os/ace/config-linux.h", "$distribution/ace/config.h");
		call_make_and_print ('', 'clean');
	}
        elsif ($os eq "darwin")
        {
                symlink ("$yarp_root/include/$os/ace/platform_macosx_panther.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
                symlink ("$yarp_root/include/$os/ace/config-macosx-panther.h", "$distribution/ace/config.h");
                call_make_and_print ('', 'clean');
        }
	print "\n";
	chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
}

if ($debug)
{
	print "\nCompiling debug\n";

	if ($os eq "winnt")
	{
		copy ("$yarp_root/include/$os/ace/config.h", "$distribution/ace/") or die "Can't copy config.h file\n"; 
		chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";

		call_msdev_and_print ("Debug", "BUILD", $project_name, $file_name);
	
		unlink "config.h";
		chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
	}
	elsif ($os eq "qnx6" && $release == 0)
	{
		chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
		symlink ("$yarp_root/include/$os/ace/platform_qnx_rtp_gcc.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
		symlink ("$yarp_root/include/$os/ace/config-qnx-rtp-62x.h", "$distribution/ace/config.h");

		call_make_and_print ('', 'debug=1 optimize=0');
	
		chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
	}
	elsif ($os eq "linux" && $release == 0)
	{
		chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
		symlink ("$yarp_root/include/$os/ace/platform_linux.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
		symlink ("$yarp_root/include/$os/ace/config-linux.h", "$distribution/ace/config.h");

		call_make_and_print ('', 'debug=1 optimize=0');

		chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
	}
        elsif ($os eq "darwin" && $release == 0)
        {
                chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
                symlink ("$yarp_root/include/$os/ace/platform_macosx_panther.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
                symlink ("$yarp_root/include/$os/ace/config-macosx-panther.h", "$distribution/ace/config.h");

                call_make_and_print ('', 'debug=1 optimize=0');
                
                chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
        }
}

if ($release)
{
	print "\nCompiling release\n";

	if ($os eq "winnt")
	{
		copy ("$yarp_root/include/$os/ace/config.h", "$distribution/ace/") or die "Can't copy config.h file\n"; 
		chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";

		call_msdev_and_print ("Release", "BUILD", $project_name, $file_name);
	
		unlink "config.h";
		chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
	}
	elsif ($os eq "qnx6")
	{
		chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
		symlink ("$yarp_root/include/$os/ace/platform_qnx_rtp_gcc.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
		symlink ("$yarp_root/include/$os/ace/config-qnx-rtp-62x.h", "$distribution/ace/config.h");

		call_make_and_print ('', 'optimize=1 debug=0');
	
		chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
	}
	elsif ($os eq "linux")
	{
		chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
		symlink ("$yarp_root/include/$os/ace/platform_linux.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
		symlink ("$yarp_root/include/$os/ace/config-linux.h", "$distribution/ace/config.h");

		call_make_and_print ('', 'optimize=1 debug=0');

		chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
	}
        elsif ($os eq "darwin")
        {
                chdir "$distribution/ace" or die "Cannot chdir to $distribution/ace: $!";
                symlink ("$yarp_root/include/$os/ace/platform_macosx_panther.GNU", "$distribution/include/makeinclude/platform_macros.GNU");
                symlink ("$yarp_root/include/$os/ace/config-macosx-panther.h", "$distribution/ace/config.h");
                call_make_and_print ('', 'optimize=1 debug=0');

                chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
        }
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

	my $output_path = './';
	if ($os eq "winnt")
	{
		$output_path = search_output($project_name);
	}

	my @my_libs = glob "$output_path/*.dll";
	foreach my $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/bin/$os/") or die "Can't copy any .dll file\n";
	}

	@my_libs = glob "$output_path/*.lib $output_path/*.a";
	foreach my $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/$os/") or die "Can't copy any .lib or .a file\n";
	}

	chdir "$current_dir" or die "Cannot chdir to $current_dir: $!";
}

select STDOUT;

#
#
#
sub search_output
{
	my ($filename) = @_;
	my $path = '';

	if (open FILE, "$distribution/ace/$filename".'.dsp')
	{
		my @matching_lines = grep { /\/out:/ } <FILE>;
		foreach my $line (@matching_lines)
		{
			if ($line =~ /\/out:\"([\w\s\\.]+)\"/)
			{
				$path = $1;
				my @pieces = split(/\\/, $path);
				
				if (@pieces > 1)
				{
					$path = $pieces[0];
					for (my $i = 1; $i < @pieces-1; $i++)
					{
						$path = join ('/', $path, $pieces[$i]);
					}
				}
				else
				{
					$path = '.';
				}
			}
		}

		close FILE;
	}

	print "the destination path is: $path\n\n";
	return $path;
}


#
# since ACE might have different project names I'm searching
# on all projects in the ace directory.
#
sub search_project
{
	foreach my $file (glob "$distribution/ace/*.dsp")
	{
		if (open FILE, "$file") 
		{
			my @matching_lines = grep { /# Microsoft Developer Studio/ } <FILE>;
			foreach my $line (@matching_lines)
			{
				if ($line =~ /Name=\"([\w \s]+)\" - /)
				{
					if ($project_name ne '')
					{
						my $candidate = $1;
						if ($candidate =~ /dll/i)
						{
							$project_name = $candidate;
							if ($file =~ /.*\/([\w \s]+.dsp)/)
							{
								$file_name = $1;
							}
						}
					}
					else
					{
						$project_name = $1;
						if ($file =~ /.*\/([\w \s]+.dsp)/)
						{
							$file_name = $1;
						}
					}
				}
			}
	
			close FILE;
		}
		else
		{
			warn "Cannot open $file: $!\n";
		}
	}
	
	print "Using $file_name for project $project_name\n\n";
	if ($project_name eq '')
	{
		die "Can't find any suitable project name to compile ACE, sorry...\n";
	}
}

#
#
#
sub call_make_and_print
{
        my ($project, $operation) = @_;

        open MK, "make $operation"."|";
        while (<MK>)
        {
                print;
        }
        close MK;
}


#
#
#
sub call_msdev_and_print
{
	my ($version, $operation, $project_name, $file_name) = @_;

	open MSDEV, "msdev $file_name /MAKE \"$project_name - Win32 ".$version."\" /".$operation."|";
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
