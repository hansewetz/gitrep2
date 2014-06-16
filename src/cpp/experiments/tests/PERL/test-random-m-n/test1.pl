#!/usr/bin/perl

#returns an array with random nGumbers between 1..n in sorted order
sub selectRandomNumbersInRange{
  my($m,$n)=@_;
  my $LARGENUMBER=189674587645;
  my @ret;
  foreach my $i (0 .. $n-1){
    my $r=int(rand($LARGENUMBER));
    if((($r%($n-$i))<$m)){
      push(@ret,$i+1);
      --$m;
    }
  }
  @ret;
}

#pick 10 random numbers out of 1 .. 1M
$seed=18;
srand($seed);
@randVec=selectRandomNumbersInRange(20,1000000);
print "@randVec \n";

