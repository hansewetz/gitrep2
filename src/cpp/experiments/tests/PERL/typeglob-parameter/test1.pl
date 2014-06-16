#!/usr/bin/perl

# take typeglob parameter - print array and scalar values
sub printArray{
  local *v=$_[0];
  print "array: @v\n";
  print "scalar $v\n";
}

# use local variable - has a typeglob (no need to pass by reference)
local @a=(1,2,3,4);
local $a=10;
printArray(a);

# use lexical scoped variable - has no type glob (must pass by reference)
my @b=(5,6,7,8,9);
printArray(\@b);
