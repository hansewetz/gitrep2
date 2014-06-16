#!/usr/bin/perl

# references are passed as parameters
sub swap{
  my ($a,$b)=@_;
  my $tmp=$$a;
  $$a=$$b;
  $$b=$tmp;
}
# params:
# v - reference to array
# l = lower index bound
# u - upper index bound
sub partAux{
  my ($v,$l,$u)=@_;
  my $m=$l;
  my $t=@$v[l];
  for ($i=$l+1;$i<=$u;++$i){
    if(@$v[$i]<$t){
      swap(\@$v[++$m],\@$v[$i]);
    }
  }
  # put pivot element in right place
  swap(\@$v[$m],\@$v[$l]);
  return $m;
}
# parameters:
# v - reference to array
sub part{
  my ($v)=@_;
  return partAux($v,0,$#$v);
}
# print array
sub printArray{
  my ($v)=@_;
  foreach $t (@$v){
    print $t," ";
  }
  print "\n";
}

# setup array to be partitioned and print it
my @v=();
$MAX = 1000000000;
for ($i=1;$i<=$MAX;++$i){
  my $j = int rand ($MAX);
  push(@v,$j);
}
# partition array and print info
my $start = time;
my $pivot=part(\@v);
my $duration = time - $start;
print "Execution time: $duration s\n";

# print partition info
print "part: ",$pivot,"\n";
#printArray(\@v);
