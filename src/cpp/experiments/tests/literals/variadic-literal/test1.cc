// see Stroustrup p.560

#include <iostream>
using namespace std;

// convert char to a digit
template<char c>
constexpr int char2no(){return c-'0';}

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
  constexpr static int val=char2no<c>()+1+add1<chars...>::val;
};
// literal adding 1 to each digit and returning the sum
template<char...c>
constexpr int operator"" _A1(){
  return add1<c...>::val;
}

// test main program
int main(){
  cout<<862_A1<<endl;
}
