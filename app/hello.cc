#include <utility/ostream.h>
#include <time.h>

using namespace EPOS;

OStream cout;
int a;
int b;

int main()
{
	a = 10;
	b = 11;
	int c = 12;
	int d = 13;
	
    cout << "--------Valores-------" << endl;
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    cout << d << endl;
    //Delay thinking(2000000);
    cout << "-------Endereços-------" << endl;
    cout << &a << endl;
    cout << &b << endl;
    cout << &c << endl;
    cout << &d << endl;
    cout << "----------FIM----------" << endl;

    return 0;
}
