#include <iostream>

namespace test_callable_object{
	// function object
	struct fobj
	{
		/* data */
		void operator()(std::string str){
			std::cout << str << std::endl;
		}
	};

	void f(std::string str){
		std::cout << str << std::endl;
	}

	void g(int& data){
		data++;
		std::cout << "mutable data = " << data << std::endl;
	}

	int main(){
		fobj fobj;
		fobj("function object!");

		using fpointer = void (*)(std::string);
		fpointer fp = f;
		fp("function pointer!");

		auto lamb = [](std::string str){
			std::cout << str << std::endl;
		};
		lamb("lambda expression!");

		auto binder1 = std::bind(f, std::placeholders::_1);
		binder1("std::bind");

		auto binder2 = std::bind(g, std::placeholders::_1);
		int data = 100;
		binder2(data);
		binder2(std::reference_wrapper<int>{data});
		std::cout << "data = " << data << std::endl;

		std::function<void(std::string)> f1 = fobj;
		f1("std::function bind function object!");

		std::function<void(std::string)> f2 = fp;
		f2("std::function bind function pointer!");

		std::function<void(std::string)> f3 = lamb;
		f3("std::function bind lambda expression!");
		return 0;
	}
}

/*
	下述代码对比了 std::bind 与 std::mem_fn 分别绑定类的成员函数时在语法上的差异
*/
namespace test_mem_fn{
	struct Widget{
		void process(){
			std::cout << "Widget process" << std::endl;
		}
	};
	int main(){
		// placeholders::_1 实际上是绑定了 this 指针
		auto binder = std::bind(&Widget::process, std::placeholders::_1);
		Widget w;
		binder(w);
		auto memf = std::mem_fn(&Widget::process);  // 这里虽然不用传递 placeholders
		memf(w);  // 但是调用时仍然需要传递 w 对象
		return 0;
	}
}

int main(){
	test_callable_object::main();
	std::cout << "--------" << std::endl;
	test_mem_fn::main();
}