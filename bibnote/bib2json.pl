#!/usr/bin/perl -w
use strict;

my %writings;
my %files;
my %scholar;

#my $httpbase = "http://biomail.ucsd.edu/labs/kristan/wagenaar/";

my %jlinks = ( "J. Negat. Results BioMed." => "www.jnrbm.com",
	       "BMC Neurosci." => "www.biomedcentral.com/bmcneurosci",
	       "Phys. Rev. E" => "pre.aps.org",
	       "J. Neurosci." => "www.jneurosci.org",
	       "J. Neurosci. Methods" => "www.elsevier.com/locate/jneumeth",
	       "J. Neural Eng." => "www.iop.org/EJ/journal/JNE",
	       "Advances in network electrophysiology using multi-electrode arrays" => "www.springer.com/west/home?SGWID=4-102-22-51202096-0&amp;changeHeader=true&amp;referer=springer.com&amp;SHORTCUT=www.springer.com/0-387-25857-4",
	       "2nd International IEEE EMBS Conference on Neural Engineering" => "www.dartmouth.edu/~ne2005/",
	       "California Institute of Technology" => "www.caltech.edu",
	       "Dept. of Mathematics, King's College London" => "www.mth.kcl.ac.uk",
	       "Dept. of Physics, University of Amsterdam" => "www.science.uva.nl",
	       "" => "",
	     );

my @lowercase = qw/in of for from at about and or to with by a the on during/;
my %lowercase; $lowercase{$_}=1 for (@lowercase);

readdb();
readfiles();
readscholar();

chdir("$ENV{HOME}/bib");
mkdir("abstracts") unless -d "abstracts";
open OUT,">bib.json" or die "Cannot open bib.json\n";
header();
body();
footer();
close OUT;

sub readfiles {
  opendir DIR, "$ENV{HOME}/papers" or opendir DIR, "$ENV{HOME}/werk/papers" or die "Cannot open papers directory\n";
  for my $f (readdir DIR) {
    chomp $f;
    $f =~ /^([CK]?\d\d-[A-Za-z0-9]+)(.*)/ and $files{$1} = $f;
  }
  closedir DIR;
}

sub readscholar {
  return if 1;
  open SCHOLAR, "scholar.txt" or die "Cannot read GScholar info\n";
  while (<SCHOLAR>) {
    chomp;
    s/^ +//;
    s/ +$//;
    next if /^$/;
    my ($id,$date,$ttl) = split(/ /,$_,3);
    $scholar{$id} = $ttl if $date>0;
  }
  close SCHOLAR;
}

sub body {
  biblio();
}

sub idcmp {
  my $a = shift;
  my $b = shift;
  $a =~ s/^[A-Z]?(\d+)/$1/;
  my $ya = $1;
  $b =~ s/^[A-Z]?(\d+)/$1/;
  my $yb = $1;
  $ya+=2000 if $ya<30;
  $yb+=2000 if $yb<30;
  $ya+=1900 if $ya<100;
  $yb+=1900 if $yb<100;
  my $yd = $ya <=> $yb;
  return $yd if $yd;
  return $a cmp $b;
}

sub biblio {
  my @arts = sort { cfauthors($a,$b) } (values %writings);
  for (sort { idcmp($a->{id}, $b->{id}) } @arts) {
    my $ty = $_->{ty};
    my $id = $_->{id};
    next if $id =~ /^D/;
    $id =~ s/^[A-Z]//;
    $_->{id} = $id; 
    $ty = "" unless defined $ty;
    $ty = lc($ty);
    if ($ty =~ /inproc/) {
      proceedings($_);
    } elsif ($ty =~ /incoll/) {
      chapters($_);
    } elsif ($ty =~ /book/) {
      books($_); #  hmmm.
    } else {
      articles($_);
    }
  }
}

sub newini {
}

sub books {
  my $a = shift;
  my $au = $a->{au};
  $au = $a->{ed} unless defined $au;
  $au = htmlify(authors($au));
  newini($au);
  print OUT "\"$a->{id}\": \"";
  backlink($a);
  print OUT htmlify($au),", ";
  unless (exists $a->{dt}) {
    $a->{re} =~ s/ *\((\d+)\)$//;
    $a->{dt} = $1;
  }
  print OUT htmlify($a->{dt}), ". ";
  print OUT "<i>", htmlify($a->{ti}), "</i>. ";
  my @rebits;
  if (exists($a->{pu}) || exists($a->{ad})) {
    push @rebits, $a->{pu} if exists($a->{pu});
    push @rebits, $a->{ad} if exists($a->{ad});
  } else {
    push @rebits, $a->{re};
  }
  $_ = htmlify($_) for (@rebits);
  print OUT join(", ", @rebits), ".";
  $a->{RE} .= ", $a->{DT}";
  morestuff($a);
  print OUT "\",\n";
}

sub chapters {
  my $a = shift;
  my $au = authors($a->{au});
  newini($au);
  print OUT "\"$a->{id}\": \"";
  backlink($a);
  print OUT htmlify($au),", ";
  print OUT htmlify($a->{dt}), ". ";
  print OUT htmlify($a->{ti}), ". ";
  $a->{RE} = "In: <i>" . jlink($a->{bt}) . "</i>";
  if (exists($a->{ed})) {
    $a->{RE} .= ", " . htmlify(authors($a->{ed})) . ", <i>eds</i>.";
  }
  $a->{RE} .= ", " . htmlify($a->{pu}) if exists($a->{pu});
  $a->{RE} .= ", " . htmlify($a->{ad}) if exists($a->{ad});
  $a->{DT} =  htmlify($a->{dt});
  $a->{PP}="";
  $a->{PP} = ", pp. " . htmlify($a->{pp}) if exists($a->{pp});
  print OUT "$a->{RE}$a->{PP}.";
  $a->{RE} .= ", $a->{DT}$a->{PP}";
  morestuff($a);
  print OUT "\",\n";
}

sub proceedings {
  my $a = shift;
  my $au = authors($a->{au});
  newini($au);
  print OUT "\"$a->{id}\": \"";
  backlink($a);
  print OUT htmlify($au),", ";
  my ($journal, $volume, $issue, $year, $pages);
  if (exists($a->{re})) {
    ($journal, $volume, $issue, $year, $pages) = parsere($a->{re});
  } else {
    $year = getyear($a->{dt});
  }
  $a->{DT} = htmlify($year);
  print OUT $a->{DT}, ". ";
  print OUT htmlify($a->{ti}), ". ";
  if (exists($a->{re})) {
    my $eds =""; $eds = htmlify(authors($a->{ed})) .", <i>eds.</i>" if exists($a->{ed});
    $a->{RE} = printre($journal, $volume, $issue, $year, $pages, $eds);
  } else {
    $a->{RE} = "<i>" . htmlify($a->{cf}) . "</i>";
    $a->{RE} .= ", " . htmlify($a->{ad}) if exists($a->{ad});
    $a->{RE} .= ", " . htmlify($a->{dt}) if exists($a->{dt});
  }
  print OUT "$a->{RE}.";
  if (exists($a->{re})) {
    $a->{RE} = printredt($journal, $volume, $issue, $year, $pages);
  }
  morestuff($a);
  print OUT "\",\n";
}

sub morestuff {
  my $a = shift;
  my $id = $a->{id};
  if (exists($a->{ab})) {
    # Create an abstract
    open AB, ">abstracts/$id.html";
    print AB <<'EOF';
<html>
  <head>
  </head>
  <body>
EOF
    print AB "<p class=\"abtitle\">";
    #print AB "<i>" if defined $a->{IT};
    print AB htmlify(titlecase($a->{ti}));
    #print AB "</i>" if defined $a->{IT};
    print AB "</p>\n";
    print AB "<p class=\"abauthors\">";
    print AB htmlify(authors($a->{au}));
    print AB "</p>\n";
    print AB "<p class=\"abreference\">";
    print AB $a->{RE};
    #my $pubm = ""; $pubm = mkpubmed($a->{pm}) if exists $a->{pm};
    #my $gsch = ""; $gsch = mkgscholar($a->{ti}) if exists($scholar{$id}) && !exists($files{$a->{id}});
    #my $ref = ""; $ref = mkref($a->{id},"../") if exists($files{$a->{id}});
    #print AB "." if $pubm || $gsch || $ref;
    #print AB " $pubm";
    #print AB " $gsch";
    #print AB " $ref";
    print AB "</p>\n";

    print AB "<p class=\"abstract\">\n";
    print AB htmlify($a->{ab},"abstract"), "\n";
    print AB "</p>\n";

    if (exists($a->{fn})) {
      my @fns = split(/;;/,$a->{fn});
      print AB "<p class=\"abstractnotes\">\n";
      for (@fns) {
        my $sym = substr($_,0,1);
        my $num = "_" . ord($sym);
        print AB "<a name=\"fn$num\"><sup>$sym</sup></a>&nbsp;", substr($_,1), "<br>\n";
      }
      print AB "</p>";
    }
    close AB;
  }
}

sub titlecase {
  my $str = shift;
  my @words = split(/ /,$str);
  for (@words) {
    substr($_,0,1) = uc(substr($_,0,1)) unless exists $lowercase{$_};
  }
  return join(" ",@words);
}

sub parsere {
  my $re = shift;
  my $journal=""; my $volume=""; my $issue = ""; my $year=""; my $pages="";
  if ($re =~ /(.*) +([A-Z]? ?\d*):?([A-Z]?[-0-9]*) *\((\d+)\) *(.*)/) {
    $journal = jlink($1);
    $volume = htmlify($2);
    $issue = htmlify($3);
    $year = htmlify($4);
    $pages = htmlify($5);
  }
  return ($journal, $volume, $issue, $year, $pages);
}

sub jlink {
  my $journal = shift;
  $journal =~ s/J\. /J\.\&nbsp;/;
  return $journal;
}

sub printre {
 my ($journal, $volume, $issue, $year, $pages, $eds) = @_;
 my $str = "<i>$journal</i>";
 if ($volume) {
   $str .= " <b>$volume";
   $str .= "($issue)" if $issue;
   $str .= "</b>";
 }
 $str .= ", $eds," if $eds;
 $str .= " $pages" if $pages;
 return $str;
}

sub printredt {
 my ($journal, $volume, $issue, $year, $pages) = @_;
 my $str = "<i>$journal</i> <b>$volume";
 $str .= ":$issue" if $issue;
 $str .= "</b> ($year), $pages";
 return $str;
}

sub backlink {
  my $a = shift;
  if (exists($a->{fn})) {
  }
}

sub articles {
  my $a = shift;
  my $au = authors($a->{au});
  newini($au);
  print OUT "\"$a->{id}\": \"";
  backlink($a);
  my ($journal, $volume, $issue, $year, $pages) = parsere($a->{re});
  print OUT htmlify($au);
  print OUT ", $year. ";
  print OUT htmlify($a->{ti}), ". ";
  $a->{RE} = printre($journal, $volume, $issue, $year, $pages);
  print OUT $a->{RE}, ".";
  $a->{RE} = printredt($journal, $volume, $issue, $year, $pages);
  morestuff($a);
  print OUT "\",\n";
}

sub getyear {
  my $str = shift;
  $str =~ /(\d+)$/ and return $1;
  return $str;
}

sub htmlify {
  my $str = shift;
  my $cls = shift;
  my @strs = split(/\$/,$str);
  $str = "";
  while (@strs) {
    $str .= htmlify1(shift(@strs),$cls);
    $str .= shift(@strs) if @strs;
  }
  $str =~ s/\\emph\{(.*)\}/<i>$1<\/i>/g;
  $str =~ s/\{//g;
  $str =~ s/\}//g;
  $str =~ s/\\\"o/ö/g;
  $str =~ s/\"/\\\"/g;
  return $str;
}

sub htmlify1 {
  my $str = shift;
  my $cls = shift;
  $cls = "" unless defined $cls;
  $cls = "class=\"$cls\"" if $cls;
  $str =~ s/\/\//\<\/p\>\<p $cls\>/g;
  $str =~ s/&/\&amp;/g;
  $str =~ s/---/\&mdash;/g;
  $str =~ s/--/\&ndash;/g;
  $str =~ s/DA\&ndash;/DA--/g;
  $str =~ s/\'\'/\&rdquo;/g;
  $str =~ s/\'/\&rsquo;/g;
  $str =~ s/\`\`/\&ldquo;/g;
  $str =~ s/\`/\&lsquo;/g;
  return $str;
}

sub authors {
  my @results;
  my $str = shift;
  $str = "" unless defined $str;
  my @authors = split(/; +/,$str);
  for $a (@authors) {
    if ($a =~ /,/) {
      my ($last, $ini) = split(/, */,$a,2);
      my @ini = split(/ +/,$ini);
      # my $initials = join(". ", split(/ +/,$ini));
      # my $fullname = "$initials" . ". " . $last;
      my $fullname = $last . " " . join("",@ini);
      push @results, $fullname;
    } else {
      my @names = split(/ +/,$a);
      my @ini;
      while (@names && $names[0] =~ /^[-A-Z]+$/) {
	push @ini, shift @names;
      }
      #my $initials = join(". ", @ini);
      my $fullname = join(" ",@names) . " " . join("",@ini);
      #my $fullname = "$initials" . ". " . join(" ", @names);
      push @results, $fullname;
    }
  }
  if (scalar(@results)==0) { 
    return "";
  } elsif (scalar(@results)==1) {
    return shift @results;
  } else {
    $results[-1] = "and " . $results[-1];
    if (scalar(@results)==2) {
      return join(" ", @results);
    } else {
      return join(", ", @results);
    }
  }
}

sub cfauthors {
  my $a = shift;
  my $b = shift;
  my $aua = authors($a->{au}) || authors($a->{ed});
  my $aub = authors($b->{au}) || authors($b->{ed});
  return lcfirst($aua) cmp lcfirst($aub) || 
      cfdate($a,$b);
}

sub cfdate {
  my $a=shift;
  my $b=shift;
  my $dta=undef;
  my $dtb=undef;
  if (exists($a->{dt})) {
    if ($a->{dt} =~ /(\d+)$/) {
      $dta = $1;
    }
  } elsif (exists($a->{re})) {
    if ($a->{re} =~ /\((\d+)\)/) {
      $dta = $1;
    }
  }
  if (exists($b->{dt})) {
    if ($b->{dt} =~ /(\d+)$/) {
      $dtb = $1;
    }
  } elsif (exists($b->{re})) {
    if ($b->{re} =~ /\((\d+)\)/) {
      $dtb = $1;
    }
  }
  $dta = 0 unless defined $dta;
  $dtb = 0 unless defined $dtb;
  return $dta <=> $dtb;
}


sub readdb {
my $ifn = "$ENV{HOME}/bib/bib.txt";
  open IN, "<$ifn" or die "Cannot read '$ifn'\n";
  my %record=();
  while (<IN>) {
    chomp;
    /^#/ and next;
    s/\t/ /g;
    s/^ +//;
    s/ +$//;
    if (/^$/) {
      if (exists($record{id})) {
	if (exists($record{ti})) {
	  $record{ti} =~ s/\.$//;
	}
	my %copy = %record;
	$writings{$record{id}} = \%copy;
      }
      %record=();
    }
    /^([A-Za-z][A-Za-z]) (.*)/ and $record{$1}=$2;
  }
}

sub header {
  print OUT "{\n";
}

sub footer {
  print OUT "\"0\": \"\" }\n";
}

