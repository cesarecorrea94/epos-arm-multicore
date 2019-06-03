#include <utility/ostream.h>

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
    cout << "-------Endereços-------" << endl;
    cout << &a << endl;
    cout << &b << endl;
    cout << &c << endl;
    cout << &d << endl;
    cout << "----------FIM----------" << endl;

    return 0;
}
