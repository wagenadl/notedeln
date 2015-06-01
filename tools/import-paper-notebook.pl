#!/usr/bin/perl -w

use strict;

my $lastline = undef;
while (<>) {
  my $line = $_;
  if ($line =~ /textblock/) {
    while (<>) {
      $line = $_;
      /^    }/ and last;
    }
    $lastline = $line;
  } else {
    print $lastline if defined $lastline;
    if ($line =~ /y0/) {
      $line = "      \"y0\": 72.0,\n";
    } elsif ($line =~ /"x"/) {
      $line = "          \"x\": -50.0,\n";
    } elsif ($line =~ /"y"/) {
      $line = "          \"y\": 0.0\n";
    } elsif ($line =~ /"scale"/) {
      $line = "          \"scale\": 0.33,\n";
    }
    $lastline = $line;
  }
}
print $lastline if defined $lastline;
