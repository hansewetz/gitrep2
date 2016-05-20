// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

// see Stroustrup p.560

#include <iostream>
using namespace std;

// convert char to a digit
template<char c>
constexpr int char2no(){return c-'0'+1;}

// primary
template<char...chars>struct add1;

// single char
template<char c>
struct add1<c>{
  constexpr static int val=char2no<c>();
};
// multiple chars
template<char c,char...chars>
struct add1<c,chars...>{
  constexpr static int val=char2no<c>()+add1<chars...>::val;
};
// literal adding 1 to each digit and returning the sum
template<char...c>
constexpr int operator"" _A1(){
  return add1<c...>::val;
}

// test main program
int main(){
  cout<<123_A1<<endl;
}
