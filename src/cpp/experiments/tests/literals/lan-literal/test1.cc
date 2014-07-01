// see Stroustrup p.558

#include <iostream>
#include <string>
using namespace std;

// test class - used for generating literals
class Lan{
public:
  Lan(char const*lan):lan_(lan){}
  Lan(Lan const&)=default;
  Lan(Lan&&)=default;
  Lan&operator=(Lan const&)=default;
  Lan&operator=(Lan&&)=default;
  ~Lan()=default;

  // debug print operator
  friend ostream&operator<<(ostream&os,Lan const&l){return os<<l.lan_<<endl;}
private:
  string lan_;
};

// lan literal
Lan operator"" _X(char const*l,size_t n){
  return Lan(l);
}

// test main program
int main(){
  Lan lan{"en"_X};
  cout<<"Language: "<<lan<<endl;
}
