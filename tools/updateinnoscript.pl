#!/usr/bin/perl -w

use strict;

sub usage {
  print STDERR "Usage: updateinnoscript.pl PACKAGE NEWVERSION\n";
  exit(1);
}

my $package = shift @ARGV or usage();
my $newvsn = shift @ARGV or usage();
usage() if @ARGV;

system("cp tools/$package-x86.iss tools/$package-x86.iss~")
  and die "Could not backup windows deployment script\n";
open IN, "<tools/$package-x86.iss~" or
  die "Could not read windows deployment script\n";
open OUT, ">tools/$package-x86.iss" or
  die "Could not read windows deployment script\n";

my $havepkg = 0;
my $havevsn = 0;
my $haveout = 0;
while (<IN>) {
  chomp;
  s/\s+$//;
  if (/AppName=(.*)/) {
    die "Package name mismatch" unless lc($1) eq lc($package);
    $havepkg = 1;
  }
  $havevsn = 1 if s/AppVersion=(.*)/AppVersion=$newvsn/;
  $haveout = 1 if s/OutputBaseName=$package-(.*)-x86-setup/OutputBaseName=$package-$newvsn-x86-setup/;
  print OUT "$_\n";
}
close IN;
close OUT;

if ($havevsn && $haveout && $havepkg) {
  exit 0;
} else {
  die "Failed to parse innoscript.\n";
}
