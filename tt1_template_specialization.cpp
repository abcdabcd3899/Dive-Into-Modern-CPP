#include <iostream>

template <typename T>
struct TestSpec{
	void print(T t){
		std::cout << "TestSpec: " << t << std::endl;
	}
};

// 模板指针特化版
template <typename T>
struct TestSpec<T*>{
	void print(T* t){
		std::cout << "TestSpec*: " << t << std::endl;
	}
};

int main(){
	TestSpec<int*> t1;
	int a = 2;
	t1.print(&a);

	TestSpec<int> t2;
	t2.print(2);
	return 0;
}