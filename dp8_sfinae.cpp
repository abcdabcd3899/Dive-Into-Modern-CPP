#include <iostream>
#include <string>
#include <memory>
#include <vector>
using namespace std;

template <typename T>
class IsClassT {
private:
	typedef char One;
	typedef struct { char a[2]; } Two;

	// C::* 成员函数指针，其实就是一个地址，这里展开就是 static One test(int address), 编译完全合法
	// 我们注意到，这两个函数没有具体的函数定义 {}, 因为我们仅仅在编译时用到
	// 只要支持作用于符号，该函数在编译时就正确
	// 这里 IsClassT 实际上存在一个漏洞，union 放在这里也会被判断为 class
	template <typename C> static One test(int C::*); //SFINAZE，当传递的参数模板参数不是类 (int,double...) 时，这个函数将出现编译错误，导致不会生成对应的调用
	template <typename C> static Two test(...);
public:
	static constexpr bool Value = sizeof( IsClassT<T>::test<T>(0)) == 1; // && !std::is_union<T>::value;  // 标准库加了是否为 union 的 traits 判断
};

struct Point
{

};

// 下面定义了两种类型的 enum，这两种都不是 class
enum class Color{
	red,
	blue,
	green,
};

enum CColor{
	red,
	blue,
	green,
};

union BigInt{
	int data;
	long value;
};

int main()
{
	cout<<std::is_class<int>::value<<endl;
	cout<<std::is_class_v<Point><<endl;
	std::cout << "--------" << std::endl;

	cout<<std::is_class_v<Color><<endl;
	cout<<std::is_class_v<CColor><<endl;
	std::cout << IsClassT<Color>::Value << std::endl;
	std::cout << IsClassT<CColor>::Value << std::endl;
	std::cout << "--------" << std::endl;

	cout<<std::is_class_v<BigInt><<endl;   // 0
	cout<<IsClassT<BigInt>::Value<<endl;   // 1  说明我们的定义错误
	std::cout << "--------" << std::endl;

	cout<<IsClassT<string>::Value<<endl;
}