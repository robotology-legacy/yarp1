#! /usr/bin/perl
#
#	--file <config_file>
#		where <config_file> is the filename of the context config file.
#
# $Id: configure.pl,v 1.14 2004-10-15 22:11:30 gmetta Exp $
# The libYARP_OS configure script.
#

use Getopt::Long;
use File::Copy;

print "Entering configure process of YARP environment...\n";

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

require "$yarp_root/conf/configure.template.pl" or die "Can't find template file $yarp_root/conf/configure.template.pl\n";

my $exp_os = check_os();

print "Ready to start...\n";

my $config_file = "$yarp_root/conf/context.conf";
my %options = ();

GetOptions ('file=s' => \$config_file );

if (-e $config_file)
{
	copy ($config_file, "$config_file.old") or warn "Can't copy $config_file, old file might be overwritten\n";

	load_config_file (\%options, $config_file);
}

#
#
#

print "Now I'm going to ask a few questions to help the configuration. ";
print "So, let's start...\n";
print "For pathnames you can use (type) the pre-defined value \$YARP_ROOT ";
print "that I've verified as: \"$yarp_root\"\n\n";
print "Please, use always the forward slash as separator!\n";

print "I determined already that you're running on a supported OS: $exp_os\n";

get_option_hash ("Architecture<-OS", $exp_os, "What's your OS?", 0, \%options);
die "Cross-compile is not supported, the auto-detected OS must be also selected\n" if ($options{"Architecture<-OS"} ne $exp_os);

get_option_hash ("Compile_OS<-ACE_PATH", "\$YARP_ROOT/src/ACE_wrappers", "Where has ACE been unpacked?", 0, \%options);
get_option_hash ("Compile_OS<-ACE_Rebuild", "NO", "Do you want to rebuild ACE, i.e. clean before building?", 1, \%options);

print "Would you like to set a default for library compilation?\n";
get_option_hash ("Compile_OS<-Lib_Clean", "FALSE", "Clean first: i.e. rebuild libraries?", 1, \%options);
get_option_hash ("Compile_OS<-Lib_Debug", "FALSE", "Debug mode?", 1, \%options);
get_option_hash ("Compile_OS<-Lib_Release", "FALSE", "Release mode (optimization on)?", 1, \%options);
get_option_hash ("Compile_OS<-Lib_Install", "FALSE", "Install after compile?", 1, \%options);
get_option_hash ("Compile_OS<-Tools_Rebuild", "YES", "Would you like to rebuild the YARP tools", 1, \%options);
get_option_hash ("Compile_OS<-Tools_Debug", "FALSE", "Would you like to compile the tools for debugging?", 1, \%options);

# consistency check.
if ($options{"Compile_OS<-Lib_Debug"} ne "TRUE" && 
	$options{"Compile_OS<-Lib_Release"} eq "FALSE" && 
	$options{"Compile_OS<-Lib_Clean"} eq "TRUE")
{
	print "Since you're rebuilding, you should at least select between debug and release\n";
	print "I'm assuming you wanted to compile debug\n";
	$options{"Compile_OS<-Lib_Debug"} = "TRUE";
}

if ($options{"Compile_OS<-Lib_Install"} eq "TRUE" &&
	$options{"Compile_OS<-Tools_Rebuild"} ne "YES")
{
	print "You need to recompile the tools since you're installing a new build of the libraries\n";
	print "I'm adding the tools compilation flag for you\n";
	$options{"Compile_OS<-Tools_Rebuild"} = "YES";
}

print "We're done for now, the context file is being created: \"$config_file\"\n";

#
# creating a new config file.
# 
save_config_file (\%options, $config_file);

print "Done!\n";
