#! /usr/bin/perl
#
#	--file <config_file>
#		where <config_file> is the filename of the context config file.
#

use Getopt::Long;
use File::Copy;

print "Entering configure process of YARP signal processing library...\n";

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

require "$yarp_root/conf/configure.template.pl" or die "Can't find template file $yarp_root/conf/configure.template.pl\n";

check_os();

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

my $os = $options{"Architecture<-OS"};

print "Now I'm going to ask a few questions to help building the configuration. ";
print "For pathnames you can use (type) the pre-defined value \$YARP_ROOT ";
print "that I've verified as: \"$yarp_root\"\n\n";
print "Please, use always the forward slash as a separator!\n";

print "I determined already that you're running on Windows\n";
die "But your configuration file doesn't report so, exiting...\n" unless ($os eq "winnt");

get_option_hash ("Compile_Sig<-IPL", "NO", "Do you want to include IPL support (emulation otherwise)?", 1, \%options);

print "Would you like to set a default for library compilation?\n";
get_option_hash ("Compile_Sig<-Lib_Clean", "FALSE", "Clean first: i.e. rebuild libraries?", 1, \%options);
get_option_hash ("Compile_Sig<-Lib_Debug", "FALSE", "Debug mode?", 1, \%options);
get_option_hash ("Compile_Sig<-Lib_Release", "FALSE", "Release mode (optimization on)?", 1, \%options);
get_option_hash ("Compile_Sig<-Lib_Install", "FALSE", "Install after compile?", 1, \%options);

# consistency check.
if ($options{"Compile_Sig<-Lib_Debug"} ne "TRUE" && 
	$options{"Compile_Sig<-Lib_Release"} ne "TRUE" && 
	$options{"Compile_Sig<-Lib_Clean"} eq "TRUE")
{
	print "Since you're rebuilding, you should at least select between debug and release\n";
	print "I'm assuming you wanted to compile debug\n";
	$options{"Compile_Sig<-Lib_Debug"} = "TRUE";
}

print "We're done for now, the context file is being updated: \"$config_file\"\n";


#
# creating a new config file.
# 
save_config_file (\%options, $config_file);

print "Done!\n";