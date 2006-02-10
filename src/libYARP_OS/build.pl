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

$yarp_root = $ENV{'YARP_ROOT'};
if (!defined($yarp_root))
{
	die "YARP_ROOT environment variable must be defined!\nto point to the path of the yarp source distribution\n";
}

require "$yarp_root/conf/configure.template.pl" or die "Can't find template file $yarp_root/conf/configure.template.pl\n";

my $exp_os = check_os();

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

print "Working with: $config_file\n";
load_config_file (\%options, $config_file);

my $os = $options{"Architecture<-OS"};

if ($os ne $exp_os)
{
	die "The auto-detected OS differs from the configured OS: $exp_os vs. $os\n";
}

if ($options{"Compile_OS<-ACE_Rebuild"} eq "YES")
{
    if (exists $options{"Compile_OS<-ACE_PATH"})
    {
        print "Looking for ACE...\n";
        $options{"Compile_OS<-ACE_PATH"} =~ s/\$YARP_ROOT/$yarp_root/;
        $options{"Compile_OS<-ACE_PATH"} =~ s#\\#/#g;
        if (-e $options{"Compile_OS<-ACE_PATH"} && -e "$yarp_root/include/$os/ace" )
        {
            if ($options{"Compile_OS<-ACE_Debug"} eq "YES") 
            {
                do_ext_compile ("$options{\"Compile_OS<-ACE_PATH\"}/build.pl --clean --debug --install --distribution $options{\"Compile_OS<-ACE_PATH\"} --os $os");
            }
            else
            {
                do_ext_compile ("$options{\"Compile_OS<-ACE_PATH\"}/build.pl --clean --debug --release --install --distribution $options{\"Compile_OS<-ACE_PATH\"} --os $os");
            }
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

	if ($os eq "winnt")
	{
		chdir "./src" or die "Cannot chdir to src: $!";

		call_msdev_and_print ("libYARP_OS", "Debug", "CLEAN");
		call_msdev_and_print ("libYARP_OS", "Release", "CLEAN");
	
		print "\n";
		chdir "../" or die "Cannot chdir to ..: $!";
	}
	elsif ($os eq "linux")
	{
		call_make_and_print ('', "clean");		
	}
	elsif ($os eq "qnx6")
	{
		call_make_and_print ('', "clean");		
	}
        elsif ($os eq "darwin")
        {
		call_make_and_print ('', "clean");
       	}
}

if ($debug)
{
	print "\nCompiling debug\n";

	if ($os eq "winnt")
	{
		chdir "./src" or die "Cannot chdir to src: $!";
		call_msdev_and_print ("libYARP_OS", "Debug", "BUILD");
		chdir "../" or die "Cannot chdir to ..: $!";
	}
	elsif ($os eq "linux")
	{
		call_make_and_print ('', "CFAST=-g");
	}
	elsif ($os eq "qnx6")
	{
		call_make_and_print ('', "CFAST=-g");
	}
	elsif ($os eq "darwin")
	{
		call_make_and_print ('', "CFAST=-g");
	}
}

if ($release)
{
	print "\nCompiling optimized\n";
	
	if ($os eq "winnt")
	{
		chdir "./src" or die "Cannot chdir to src: $!";
		call_msdev_and_print ("libYARP_OS", "Release", "BUILD");
		chdir ".." or die "Cannot chdir to ..: $!";
	}
	elsif ($os eq "linux")
	{
		call_make_and_print ('', "CFAST=-O3");
	}
	elsif ($os eq "qnx6")
	{
		call_make_and_print ('', "CFAST=-O3");
	}
	elsif ($os eq "darwin")
	{
		call_make_and_print ('', "CFAST=-O2");
	}
}

if ($install)
{
	print "\nInstalling YARP libraries to default install directory.\n";
	print "Later this might change to something smarter.\n";
	@my_headers = glob "./include/yarp/*.h";
	foreach $file (@my_headers) 
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/include/yarp/") or warn "Can't copy $file\n"; 
	}

	@my_libs = glob "./lib/$os/*.lib ./lib/$os/*.a";
	foreach $file (@my_libs)
	{
		print "Copying $file\n";
		copy ($file, "$yarp_root/lib/$os/") or warn "Can't copy $file\n";
		if ($os eq "darwin")
		{
			print `ar -s $yarp_root/$file`; 
		}
	}
}

if ((!$force && $options{"Compile_OS<-Tools_Rebuild"} eq "YES") || 
	($force && $tools))
{
	if ($options{"Compile_OS<-Tools_Debug"} eq "TRUE" || (!$release && $force))
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_ext_compile ("build-os.pl --clean --debug --install --os $os");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
	else
	{
		my $current_dir = getcwd;
		chdir "../tools/" or die "Can't chdir to tools directory\n";
		do_ext_compile ("build-os.pl --clean --release --install --os $os");
		chdir $current_dir or die "Can't chdir to $current_dir\n";
	}
}
else
{
	print "You didn't ask to recompile the YARP tools\n";
}
