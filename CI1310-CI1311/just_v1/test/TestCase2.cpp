#include <iostream.h>
class foo{int x;
public:
foo(int _x):x(_x) {}~foo() { cout << "I am destroyed: " << x << endl; }};foo z(1);
main(){
// Los comentarios deben quedar tal y como estan, ;}, :}, ;)
// No hacer caso de comillas " u otras cosas ' { ( `
foo * x = new foo(2);delete x;foo y(3);}
