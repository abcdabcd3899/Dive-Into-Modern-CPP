#include <utility>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;



class MyClass {
	public:

	void print(int data){
		cout<<"MyClass print: "<<data<<endl;
	}

	static void sprint(int data){
		cout<<"MyClass static sprint: "<<data<<endl;
	}
};

void print(int data){
	cout<<"print function pointer: "<<data<<endl;
}

struct Printer {
	void operator() (int data) const {
		cout  << "Print Functor: "<< data <<endl;
	}
};

int main()
{
	MyClass mc;

	cout<<"invoke---------"<<endl;
	{
		std::invoke(print, 10 );
		std::invoke(Printer{}, 20 );
		std::invoke(&MyClass::print, &mc, 30 );
		std::invoke(&MyClass::print, mc, 30);
		std::invoke(&MyClass::sprint,40);
		std::invoke(
		[](int data){cout<<"lambda:"<<data<<endl;},
		50);

	}

	cout<<"function---------"<<endl;
	{
		std::function func1=print;
		std::function func2=Printer{};
		std::function<void(MyClass, int)> func3_1=&MyClass::print;
		std::function<void(MyClass*, int)> func3_2=&MyClass::print;
		std::function func4=&MyClass::sprint;
		std::function func5=[](int data){cout<<"lambda:"<<data<<endl;};

		func1(100);
		func2(200);
		func3_1(mc,300);
		func3_2(&mc,300);
		func4(400);
		func5(500);

		std::invoke(func5,500);
	}
}

