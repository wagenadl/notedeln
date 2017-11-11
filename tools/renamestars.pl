#!/usr/bin/perl -w

use strict;
use JSON;

for (@ARGV) {
  process($_);
}

sub process {
  my $pgfile = shift;

  $pgfile =~ s/\.res\/?/\.json/;

  my $resdir = $pgfile;
  $resdir =~ s/\.json/\.res/;

  open PG, "<$pgfile" or die;
  my $pg = join("", <PG>);
  close PG;
  my $json = decode_json($pg);

  my @renames;

  findstars($json, \@renames);

  $pg = to_json($json, {utf8=>1, pretty=>1});

  print "$pg";
  print "$pgfile $resdir\n";
  print @renames;

  if (@renames) {
    system("cp $pgfile $pgfile~") and die;
    open PG, ">$pgfile" or die;
    print PG $pg;
    close PG;
    for my $out (@renames) {
      my $in = $out;
      $in =~ s/^star/\*/;
      my $cmd = "mv $resdir/$in $resdir/$out";
      print "$cmd\n";
      system($cmd);
    }
  }
}

######################################################################
sub findstars {
  my $json = shift;
  my $renames = shift;
  
  my $tp = ref($json);
  if ($tp eq "HASH") {
    for my $k (keys %$json) {
      if ($k eq "archive" || $k eq "tag" || $k eq "resName") {
	if ($json->{$k} =~ s/^\*/star/) {
	  push @$renames, $json->{$k} if $k eq "archive";
	}
      } else {
	findstars($json->{$k}, $renames);
      }
    }
  } elsif ($tp eq "ARRAY") {
    findstars($_, $renames) for (@$json);
  }
}
