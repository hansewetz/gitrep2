#!/usr/bin/perl
use English;

sub printFile{
  my ($infile)=(@_);
  open(IN_FILE,$infile);
  my @lines=<IN_FILE>;
  close(IN_FILE);
  my %w2ind;
  foreach my $line (@lines){
    $line =~ s/l+//;
    my @words=split(/\s+/,$line);
    $w2ind{$words[0]}=$words[1];
  }
  foreach my $key (keys %w2ind){
    print $key." ==> ".$w2ind{$key}."\n";
  }
}
printFile("junk.txt");
