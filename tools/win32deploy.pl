#!/usr/bin/perl -w

use strict;
use Cwd;
use File::Copy;
use File::Path;

######################################################################
# EXTERNAL PATHS
my $qt_path = "c:/Qt/Qt5.7.1-x86";
my $qbin_path = $qt_path . "/5.7/msvc2015/bin";
my $qssl_path = $qt_path . "/Tools/QtCreator/bin";
my $openssl_path = "c:/OpenSSL-Win32";
my $msvc_path = "c:/Program Files (x86)/Microsoft Visual Studio 14.0/VC";

######################################################################
# INTERNAL PATHS
my $eln_buildpath = "build-eln-x86/release";
my $webgrab_buildpath = "build-webgrab-x86/release";
my $release_path = "release-eln-x86";

######################################################################

$msvc_path =~ s/\//\\/g;
$ENV{VCINSTALLDIR} = $msvc_path;

die "eln executable not found" unless -f "$eln_buildpath/eln.exe";
die "webgrab executable not found" unless -f "$webgrab_buildpath/webgrab.exe";


File::Path::remove_tree($release_path) if -d $release_path;
File::Path::make_path($release_path);

system("$qbin_path/windeployqt --dir $release_path "
       . " --compiler-runtime $eln_buildpath/eln.exe")
  and die "Failed to get eln deployment";

system("$qbin_path/windeployqt --dir $release_path "
       . " --compiler-runtime $webgrab_buildpath/webgrab.exe")
  and die "Failed to get webgrab deployment";

File::Copy::copy("$eln_buildpath/eln.exe", "$release_path/");
File::Copy::copy("$webgrab_buildpath/webgrab.exe", "$release_path/");

sub sslcopy {
  my $pth = shift;
  my $fn = shift;
  print "$fn not found in $pth - no https support\n" unless -f "$pth/$fn";
  File::Copy::copy("$pth/$fn", "$release_path/"); 
}

sslcopy($qssl_path, "ssleay32.dll", "$release_path/");
sslcopy($qssl_path, "libeay32.dll", "$release_path/");
sslcopy($openssl_path, "libcrypto-1_1.dll", "$release_path/");
sslcopy($openssl_path, "libssl-1_1.dll", "$release_path/");

print "Now run 'tools/eln-x86.iss' using Inno Setup.\n";
