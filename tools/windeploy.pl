#!/usr/bin/perl -w

use strict;
use Cwd;
use File::Copy;
use File::Path;

######################################################################
# EXTERNAL PATHS
my $qbin_path = "c:/Qt/Qt5.7.0-x86/5.7/msvc2013/bin";
my $msvc_path = "c:/Program Files (x86)/Microsoft Visual Studio 12.0/VC";

######################################################################
# INTERNAL PATHS
my $eln_buildpath = "build-eln-x64/release";
my $webgrab_buildpath = "build-webgrab-x64/release";
my $release_path = "release-eln-x64";

######################################################################

$msvc_path =~ s/\//\\/g;
$ENV{VCINSTALLDIR} = $msvc_path;

File::Path::remove_tree($release_path) if -d $release_path;
File::Path::make_path($release_path);

system("$qbinpath/windeployqt --dir $release_path "
       . " --compiler-runtime $eln_buildpath/eln.exe")
  and die "Failed to get eln deployment";

system("$qbinpath/windeployqt --dir $release_path "
       . " --compiler-runtime $webgrab_buildpath/webgrab.exe")
  and die "Failed to get webgrab deployment";

File::Copy::copy("$eln_buildpath/eln.exe", "$releasepath/");
File::Copy::copy("$webgrab_buildpath/webgrab.exe", "$releasepath/");

print "Now run 'tools/eln-x64.iss' using Inno Setup.\n";
