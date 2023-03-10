#include <iostream>
#include <vector>
#include <algorithm>  // for_each

namespace test_lambda{
	void print(){
		std::vector<int> v{1,2,3,4,5,6,7};
		std::for_each(v.begin(), v.end(), [](int i){
			i++;
			std::cout << i << " ";
			});
		std::cout << std::endl;
		for(auto &i : v){
			std::cout << i << " ";
		}
		std::cout << std::endl;
		std::for_each(v.begin(), v.end(), [](int &i){
			i++;
			std::cout << i << " ";
			});
		std::cout << std::endl;
		for(auto &i : v){
			std::cout << i << " ";
		}
		std::cout << std::endl;

	}
	int main(){
		print();
	return 0;
	}
};

namespace test_introducer{
	using FilterContainer = std::vector<std::function<bool()>>;
	FilterContainer filters;
	class Widget{
	public:
		void addFilter() const;
		Widget(int _w): divisor(_w) {};
	public:
		int divisor;
    };

	void Widget::addFilter() const{
		// int divisorCopy = divisor;
		// std::cout << "&divisorCopy = " << &divisorCopy << " " <<"&divisor = " << &divisor << std::endl;
		// std::cout <<"&divisor = " << &divisor << std::endl;
		// std::cout << "divisorCopy = " << divisor << " " << "divisor = " << this->divisor << std::endl;
		// std::cout << "divisor = " << divisor << std::endl;
		std::cout << "----------" << std::endl;
		// filters.emplace_back([divisor = divisor]() -> bool{
		// filters.emplace_back([&divisorCopy, this]() -> bool{
		// filters.emplace_back([=]() -> bool{
		// filters.emplace_back([&]() -> bool{
		filters.emplace_back([divisorCopy = divisor, this]() -> bool{
			std::cout << "&divisorCopy = " << &divisorCopy << " " <<"&divisor = " << &divisor << std::endl;
			// std::cout <<"&divisor = " << &divisor << std::endl;
			std::cout << "divisorCopy = " << divisorCopy << " " << "divisor = " << this->divisor << std::endl;
			// std::cout << "divisor = " << divisor << std::endl;
			return 1000 % divisor == 0;
		});
	}
	/*
		如果我们去掉 doSomework(), 将其逻辑移动到 main 函数中,那么上面我们采用 & 或者 = 捕获就能得到正确的结果.
	*/
	void doSomework(){
		auto pw = new Widget(1000);
		pw->addFilter();
	}

	int main(){
		doSomework();
		// auto pw = new Widget(1000);
		// pw->addFilter();
		filters[0]();
		return 0;
	}
}


/*
	这个测试用来证明,gi 能被访问到不是因为 lambda 补货到了 gi,而是因为其是一个全局变量
	到底哪些变量能被捕获,参见下面链接
	https://en.cppreference.com/w/cpp/language/lambda 中 Explianation 部分
*/
int gi = 30;
namespace test_introducer_global_variable{
	int main(){
		int j = 30;
		{
			// auto lamb1 = [gi, j]()mutable{
			// 	std::cout << "&j = " << &j << ", j = " << ++j << ", &gi = " << &gi << ", gi = " << gi << std::endl;
			// };  // error: 'gi' cannot be captured because it does not have automatic storage duration
			// auto lamb2 = [&gi, &j](){
			// 	std::cout << "&j = " << &j << ", j = " << ++j << ", &gi = " << &gi << ", gi = " << gi << std::endl;
			// }; // error: 'gi' cannot be captured because it does not have automatic storage duration
			auto lamb3 = [=]()mutable{
				std::cout << "&j = " << &j << ", j = " << ++j << ", &gi = " << &gi << ", gi = " << gi << std::endl;
			};
			auto lamb4 = [&](){
				std::cout << "&j = " << &j << ", j = " << ++j << ", &gi = " << &gi << ", gi = " << gi << std::endl;
			};
			lamb3();
			std::cout << "&j = " << &j << ", j = " << ++j << ", &gi = " << &gi << ", gi = " << gi << std::endl;
			lamb4();
			std::cout << "&j = " << &j << ", j = " << ++j << ", &gi = " << &gi << ", gi = " << gi << std::endl;
		}
		return 0;
	}
}

namespace test_lambda_closure_class{
	int main(){
		auto lamb = [](){std::cout << __PRETTY_FUNCTION__ << ", " << __LINE__ << ", " << std::endl;};  // print the signature of the function and code line
		lamb();  // auto test_lambda_closure_class::main()::(anonymous class)::operator()() const, 115,
		return 0;
	}
}

namespace test_lambda_copy{
	int main(){
		int x = 0;
		auto lamb1 = [&x](){std::cout << __PRETTY_FUNCTION__ << ", " << __LINE__ << ", " << "x = " << x++ << std::endl;};
		auto lamb2 = lamb1;
		auto lamb3 = lamb2;

		lamb1();
		lamb2();
		lamb3();

		return 0;
	}
	/*
		auto test_lambda_copy::main()::(anonymous class)::operator()() const, 124, x = 0
		auto test_lambda_copy::main()::(anonymous class)::operator()() const, 124, x = 1
		auto test_lambda_copy::main()::(anonymous class)::operator()() const, 124, x = 2
	*/
}

namespace test_function_template_lambda{
	// functor https://www.bogotobogo.com/cplusplus/functors.php
	// https://stackoverflow.com/questions/317450/why-override-operator#317528
	// https://stackoverflow.com/questions/356950/what-are-c-functors-and-their-uses
	// https://marquistj13.github.io/MyBlog/ReadingNotes/ProgrammingSkills/cplusplus/basic%20concepts%20%20functor
	template <typename T>
	struct PrintTF{
		public:
		void operator()(const T & i) const{
			std::cout << i << " ";
		}
	};

	// function template
	template <typename T>
	void printF(const T& i){
		std::cout << i << " ";
	}

	// function
	void print(const int& i){
		std::cout << i << " ";
	}

	auto lamb = [](const auto& i){ std::cout << i << " "; };

	int main(){
		std::vector<int> v {1, 3, 5};
		std::for_each(v.begin(), v.end(), [](int &i){
			i++;
			std::cout << i << " ";
			});  // lambda expression
		std::cout << std::endl;

		std::for_each(v.begin(), v.end(), PrintTF<int>());  // functor
		std::cout << std::endl;

		std::for_each(v.begin(), v.end(), printF<int>);  // function template
		std::cout << std::endl;

		std::for_each(v.begin(), v.end(), print);  // function
		std::cout << std::endl;

		std::for_each(v.begin(), v.end(), lamb);  // function
		std::cout << std::endl;
		return 0;
	}
}

/*
with f1, we specify that we want to capture everything
that we need by value; this does not "capture" the value
of g at the time we declare it because g is globally availabe,
there is only one copy 

with f2, we explicitly say that we want to capture g
by value at the point the lambda is delcared, so the 
value of g at the time of the declaration is what is
captured by the lambda, and it does not reflect the 
value after it is updated in the body of main()
*/

// naturally, these print different values; what happened?
int g = 10;
namespace test_puzzle{
	struct Test_This{
		int x = 100;
		int y = 20;
	};

	int main()
	{
		auto f1 = [=]() {
				std::cout << "Function Name: " << __PRETTY_FUNCTION__ << " ,Line: " << __LINE__ << std::endl;
				return g + 1;
			};
		auto f2 = [new_g {g}](){  // new_g 是闭包类的成员变量? Yes
				std::cout << "Function Name: " << __PRETTY_FUNCTION__ << " ,Line: " << __LINE__ << std::endl;
				std::cout << "typeid: " << typeid(new_g).name() << std::endl;
				return new_g + 1;
			};
		g = 20;
		auto f3 = [g=g](){ return g + 1; };

		std::cout << "f1() = " << f1() << std::endl;  // 21
		std::cout << "f2() = " << f2() << std::endl;  // 11
		std::cout << "f3() = " << f3() << std::endl;  // 21

		// 这个语法我们称为带有初始化表达式的初始化捕获
		auto f4 = [g = 1, value = 10](){return g+value;};  // An initializer expression in a capture list is also called an init capture.
		std::cout << "f3() = " << f4() << std::endl;  // 11
		auto f5 = [new_x = std::move(std::make_shared<Test_This>()->x)](){  // new_g 是闭包类的成员变量? Yes
				std::cout << "Function Name: " << __PRETTY_FUNCTION__ << " ,Line: " << __LINE__ << std::endl;
				return new_x + 1;
		};
		std::cout << "f5() = " << f5() << std::endl;
		return 0;
	}
}

namespace test_bind_object_pass_by_value{
	using namespace std::placeholders;
	void func(int a, int &b){
		a++;
		b++;
		std::cout << "a = " << a << ", b = " << b <<std::endl;
	}
	int main(){
		int a = 1;
		int b = 2;
		std::cout << "a = " << a << ", b = " << b <<std::endl;

		auto f = std::bind(func, _1, std::ref(b));  // pass by reference using std::ref
		f(a);  // call f bind object. a pass to placeholder _1 for bind object f
		std::cout << "a = " << a << ", b = " << b <<std::endl;
		return 0;
	}
}

namespace test_bind_object_pass_by_ref{
	using namespace std::placeholders;
	void func(int &a, int &b){
		a++;
		b++;
		std::cout << "a = " << a << ", b = " << b <<std::endl;
	}
	int main(){
		int a = 1;
		int b = 2;
		std::cout << "a = " << a << ", b = " << b <<std::endl;

		auto f = std::bind(func, _1, std::ref(b));  // pass by reference using std::ref
		f(a);  // call f bind object
		std::cout << "a = " << a << ", b = " << b <<std::endl;
		return 0;
	}
}

int main(){
	// test_lambda::main();
	// test_introducer::main();
	// test_introducer_global_variable::main();
	// test_lambda_closure_class::main();
	// test_lambda_copy::main();
	// test_function_template_lambda::main();
	// test_puzzle::main();
	test_bind_object_pass_by_value::main();
	std::cout << "-----------" << std::endl;
	test_bind_object_pass_by_ref::main();
	return 0;
}
