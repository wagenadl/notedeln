#!/usr/bin/perl -w
use strict;

my @fns = glob("*.json");
my %dates;
my %pages;

for my $fn (@fns) {
  open JSON, "<$fn" or die;
  $fn =~ /^(\d+)/;
  $pages{$fn} = $1;
  while (<JSON>) {
    chomp;
    if (/"cre": "(.*)"/) {
      $dates{$fn} = $1;
      last;
    }
  }
  close JSON;
}

my %newpage;

my $ord = 0;
for my $fn (sort { $dates{$a} cmp $dates{$b}} @fns) {
  $newpage{$fn} = ++$ord;
}

for my $fn (@fns) {
  #print "$fn => $dates{$fn} => $newpage{$fn}\n";
  my $notefn = $fn;
  $notefn =~ s/json/notes/;
  my $resfn = $fn;
  $resfn =~ s/json/res/;

  my $newpg = $newpage{$fn};
  my $oldpg = $pages{$fn} + 0;
  system("perl -ibak -pe 's/\"startPage\": $oldpg,/\"startPage\": $newpg,/' $fn");
  my $oldpg = $pages{$fn};
  $newpg = sprintf("%04i", $newpg);
  my $newfn = $fn;
  $newfn =~ s/$oldpg/$newpg/;
  system("git mv $fn $newfn");
  if (-d $notefn) {
    my $newfn = $notefn;
    $newfn =~ s/$oldpg/$newpg/;
    system("git mv $notefn $newfn");
  }
  if (-d $resfn) {
    my $newfn = $resfn;
    $newfn =~ s/$oldpg/$newpg/;
    system("git mv $resfn $newfn");
  }
}

