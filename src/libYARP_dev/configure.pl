#! /usr/bin/perl
#
#	--file <config_file>
#		where <config_file> is the filename of the context config file.
#

use Getopt::Long;
use File::Copy;

print "Entering configure process of YARP device drivers...\n";

chomp ($ver = `ver`);
chomp ($uname = `uname`);
if (index ($ver, "Windows") < 0 && index ($uname, "uname") < 0)
{
	print "This is a Windows 2000/XP specific script\n";
	print "Perhaps this procedure can be simply extended to\n"; 
	print "other OSes but for now, this is all experimental...\n";
	
	die "This script is specific to Windows 2000/XP and Cygwin\n";
}

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

print "Ready to start...\n";

my $config_file = "$yarp_root/conf/context.conf";
my %options = ();

GetOptions ('file=s' => \$config_file );

if (-e $config_file)
{
	copy ($config_file, "$config_file.old");

	open CONFIG, $config_file or die "Can't open config file $!";

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
}

#
#
#
my $os = $options{"Architecture<-OS"};

#
#
#
print "Now I'm going to ask a few questions to help that I need for configuring the device drivers\n";
print "For pathnames you can use (type) the pre-defined value \$YARP_ROOT ";
print " that I've verified as: \"$yarp_root\"\n\n";
print "Please, use always the forward slash as a separator!\n";

print "I determined already that you're running on Windows. ";
die "But, your config file doesn't report so\n" unless ($os eq "winnt");

print "I also imagine you've compiled YARP_OS, I'm not checking for it so please ";
print "make sure you've run \"configure.pl\" and \"build.pl\" for YARP_OS\n";

print "You are going to provide information on your hardware configuration here. ";
print "If this procedure isn't clear to you, please, have a look at the documentation\n";

print "To start, provide a name for your hardware context. This will be merged with the ";
print "OS name to provide our installation means of distinguishing this specific hardware\n";

get_option_hash ("Architecture<-Hardware_Name", "Null", "What is your hardware name?");
print "Then your context specific directories are going to be called: \"$options{\"Architecture<-Hardware_Name\"}_$os\n";

print "These are the standard flags for compiling the library\n";

get_option_hash ("Compile_Dev<-Lib_Clean", "FALSE", "Clean first: i.e. rebuild libraries?");
get_option_hash ("Compile_Dev<-Lib_Debug", "FALSE", "Compile debug version?");
get_option_hash ("Compile_Dev<-Lib_Release", "FALSE", "Compile release (optimized)?");
get_option_hash ("Compile_Dev<-Lib_Install", "FALSE", "Install after build?");

if ($options{"Compile_Dev<-Lib_Clean"} eq "TRUE" &&
	$options{"Compile_Dev<-Lib_Debug"} eq "FALSE" &&
	$options{"Compile_Dev<-Lib_Release"} eq "FALSE" &&
	$options{"Compile_Dev<-Lib_Install"} eq "TRUE")
{
	print "Asked to clean and install but not to rebuild anything, I assume debug compile\n";
	$options{"Compile_Dev<-Lib_Debug"} = "TRUE";
}

print "Browsing through the list of available device drivers\n";
print "In case you don't know, it's no harm including all available drivers\n";

open PROJECT, "./src/libYARP_dev.dsp" or die "Can't open project file: $!\n";
$newname = "libYARP_dev_$options{\"Architecture<-Hardware_Name\"}";
open MYPROJECT, "> ./src/$newname.dsp" or die "Can't open output project file: $!\n";

my $stop_copying = 0;

while (<PROJECT>)
{
	s/libYARP_dev/$newname/g;
	if (/# Name "$newname - Win32 Debug"/)
	{
		print MYPROJECT $_;
		$stop_copying = 1;
	}
	elsif (/# Begin Group "Source Files"/)
	{
		print MYPROJECT $_;
		my $line = undef;
		while (1)
		{
			$line = <PROJECT>;
			print MYPROJECT $line;
			if ($line =~ /^# End Group/)
			{
				last;
			}
		}
	}
	elsif (/# Begin Group "Header Files"/)
	{
		print MYPROJECT $_;
		my $line = undef;
		while (1)
		{
			$line = <PROJECT>;
			print MYPROJECT $line;
			if ($line =~ /^# End Group/)
			{
				last;
			}
		}
	}
	else
	{
		if (!$stop_copying)
		{
			print MYPROJECT $_;
		}
	}
}

foreach my $device (glob "*")
{
	if (-d "$device/$os/yarp")
	{
		print "Would you like to add \"$device\" to the project [NO]? ";
		chomp(my $answer = <STDIN>);
		if ($answer eq "YES")
		{
			$options{"Compile_Dev<-DD_$device"} = "YES";
			print MYPROJECT "# Begin Group \"$device\"\n\n";
			print MYPROJECT "# PROP Default_Filter \"cpp;h\"\n";
			
			foreach my $file (glob "$device/$os/yarp/*.cpp $device/$os/yarp/*.h")
			{
				print MYPROJECT "# Begin Source File\n\n";
				$file =~ s#/#\\#g;
				my $line = "SOURCE=..\\$file\n";
				print MYPROJECT "$line";
				print MYPROJECT "# End Source File\n";
			}

			print MYPROJECT "# End Group\n";
		}
		else
		{
			$options{"Compile_Dev<-DD_$device"} = "NO";
		}
	}
}

print MYPROJECT "# End Target\n# End Project\n";

close MYPROJECT;
close PROJECT;

print "We're done for now, the context file has been updated: \"$config_file\"\n";
print "A new project reflecting your choices has been created in \"./src\"\n";
print "Type \"build.pl\" to start the build process\n";

#
# uses global value %options
#
sub get_option_hash
{
	my ($key, $default_value, $message) = @_;
	my $line = undef;

	$options{$key} = $default_value if (!exists($options{$key}));

	print "$message [$options{$key}] ";
	chomp($line = <STDIN>);
	$options{$key} = $line if (defined($line) && $line ne '');

	0;
}


#
# creating a new config file.
# 
open CONFIG, "> $config_file";
select CONFIG;

print "//\n";
print "// This is an example of configuration file.\n";
print "// - projects and include files are created relying on this options.\n";
print "//\n";
print "// Generated by <configure.pl>\n";
print "//\n";

my $context = '';
foreach my $key (sort keys %options)
{
	my $value = $options{$key};
	$key =~ /<-/;
	if ($context ne $`)
	{
		$context = $`;
		print "\n[$context]\n";
	}
	print "$'= $value\n";
}

select STDOUT;
close CONFIG;

print "Done!\n";