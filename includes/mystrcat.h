#include <sstream>
#include <iostream>
#include <string>
 
using namespace std;
 
class Mystrcat{
public:
  template<typename T, typename ...P>
  explicit Mystrcat(T t, P... p){init(t,p...);}
 
  operator const string(){return o.str();}
 
private:
  ostringstream o;
  void init(){}
  template<typename T, typename ...P>
  void init(T t, P... p);
};
 
template<typename T, typename ...P>
void Mystrcat::init(T t, P ...p){
  o << t << ' ';
  init(p...);
}
