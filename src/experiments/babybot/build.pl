#! /usr/bin/perl
#
# compiling Babybot code
#
# options --debug compile the DEBUG version
#		  --release to compile optimized
#		  --clean to clean obj files
#		  --install to copy files to the defaul installation path
#
#

use Getopt::Long;
use File::Copy;
use Cwd;

print "Entering compile process of Babybot code...\n";

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
my $os = 'winnt';
my $ipl = '';

GetOptions ('debug' => \$debug,
            'release' => \$release,
			'clean' => \$clean,
			'install' => \$install,
			'ipl' => \$ipl);

print "Compiling libraries first. ";
print "Assuming a standard format wiht /src /include directories\n";

if ($ipl && -d "ipl")
{
	print "Installing IPL to standard path...\n";

	if (! -d "$yarp_root/include/ipl")
	{
		mkdir ("$yarp_root/include/ipl") or warn "Can't create ipl directory in $yarp_root/include\n";
	}

	chdir "./ipl/include" or warn "Can't chdir to ipl/include: $!\n";
	foreach $file (glob "*.h")
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/ipl/");
	}
	chdir ".." or warn "Can't chdir to ..: $1\n";

	chdir "./lib" or warn "Can't chdir to lib: $!\n";
	foreach $file (glob "*.lib")
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/$os/");
	}
	chdir ".." or warn "Can't chdir to ..: $1\n";

	chdir "./bin" or warn "Can't chdir to bin: $!\n";
	foreach $file (glob "*.dll")
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/bin/$os/");
	}
	chdir "../.." or warn "Can't chdir to ../..: $1\n";
}

foreach $directory (glob "*")
{
	if (-d "$directory" && -d "$directory/src" && -d "$directory/include/yarp")
	{
		# it's likely to be a library.
		if (!chdir "$directory")
		{
			warn "Can't chdir to $directory\n";
			next;
		}
		
		foreach $project (glob "*.dsp")
		{
			my $name = '';
			if ($project =~ /(.+).dsp/)
			{
				$name = $1;
			}

			if ($clean)
			{
				print "\nCleaning $project in $directory\n";

				call_msdev_and_print ($name, "Debug", "CLEAN");
				call_msdev_and_print ($name, "Release", "CLEAN");
				
				print "\n";
			}

			if ($debug)
			{
				print "\nCompiling debug\n";
				call_msdev_and_print ($name, "Debug", "BUILD");
			}

			if ($release)
			{
				print "\nCompiling optimized\n";
				call_msdev_and_print ($name, "Release", "BUILD");
			}

			if ($install)
			{
				print "\nInstalling YARP library to default install directory.\n";
				foreach $file (glob "./include/yarp/*.h ./include/yarp/*.inl") 
				{
					print "Copying $file\n";
					copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy $file: $!\n"; 
				}

				foreach $file (glob "./lib/$os/*.lib")
				{
					print "Copying $file\n";
					copy ($file, "$yarp_root/lib/$os/") or warn "Can't copy $file: $!\n";
				}
			}

		}
		
		chdir ".." or warn "Can't chdir to ..\n";
	}
}

foreach $directory (qw/ headcontrol armcontrol /) #(glob "*")
{
	if (-d $directory && !(-d "$directory/src") && !(-d "$directory/include/yarp"))
	{
		if (!chdir $directory)
		{
			warn "Can't chdir to $directory\n";
			next;
		}

		foreach $project (glob "*.dsp")
		{
			my $name = '';
			if ($project =~ /(.+).dsp/)
			{
				$name = $1;
			}

			if ($clean)
			{
				print "\nCleaning $project in $directory\n";

				call_msdev_and_print ($name, "Debug", "CLEAN");
				call_msdev_and_print ($name, "Release", "CLEAN");
				
				print "\n";
			}

			if ($debug)
			{
				print "\nCompiling debug\n";
				call_msdev_and_print ($name, "Debug", "BUILD");
			}

			if ($release)
			{
				print "\nCompiling optimized\n";
				call_msdev_and_print ($name, "Release", "BUILD");
			}

			if ($install)
			{
				print "\nInstalling YARP application to default path.\n";
				$searchfor = ($debug) ? "Debug" : (($release) ? "Release" : '');
				if ($searchfor)
				{
					foreach $file (glob "./$searchfor/*.exe") 
					{
						print "Copying $file\n";
						copy ($file, "$yarp_root/bin/$os") or warn "Can't copy $file: $!\n"; 
					}
				}
			}

		}

		chdir "../" or warn "Cannot chdir to ..: $!";
	}
}

print "\nDone!\n";

sub call_msdev_and_print
{
	my ($name, $version, $operation) = @_;

	open MSDEV, "msdev $name.dsp /MAKE \"$name - Win32 ".$version."\" /".$operation."|";
	while (<MSDEV>)
	{
		print;
	}
	close MSDEV;	
}




