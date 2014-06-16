#!/usr/bin/perl
use English;

unlink("./junk.txt");
if($ERRNO!=0){
  print "errno: $ERRNO\n";
  exit 1;
}
