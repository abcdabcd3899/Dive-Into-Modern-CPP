### lambda

lambda 应该是 function programming 带给 C++ 最大的改变之一,但是 lambda 的加入并没有丰富 C++ 语言
的表达能力,因为所有可用 lambda 表达的部分都可以写一个函数指针或者函数对象来代替,但是:
**lambda 更容易被内联.相比函数指针而言减少了调用代价.** 同样 lambda 作为一种创建**函数对象**的手段,
它的便捷性是 std::function 不能比的. 这对日常软件的开发产生了极大的影响. 如果不是有了 lambda 表达
式, STL 中的 `_if` 结尾的算法 (std::algorithm) 恐怕只能使用函数对象、函数指针等来做了. 同样在开发中,
我们经常用 lambda 表达式作为回调函数.有了 function programming 中的 lambda, C++ 的语言确实
感觉更便捷了.

#### lambda 名词解释

* lambda 表达式
lambda 表达式都是以 `[]` 开始,满足下面的形式,都被称为 lambda 表达式.
* [] 中的内容称为 lambda introducer. 捕获列表
* () lambda 表达式形式参数.
* {} lambda 表达式的定义.
* -> type lambda 返回值

```C++
[](){std::cout << "lambda" << std::endl; }
```

* 闭包
闭包是 lambda 表达式创建的**运行期**对象,根据不同的捕获 (lambda introducer) 模式, 闭包会持有数据的
副本或者引用. 下面例子中,传入 `std::for_each` 的实际上就是闭包对象.

```C++
std::for_each(v.begin(), v.end(), [](int i){
    i++;
    std::cout << i << " ";
   });
std::for_each(v.begin(), v.end(), [](int &i){
    i++;
    std::cout << i << " ";
   });
```

* 闭包类

闭包类就是实例化闭包的类.每个 lambda 表达式都会触发编译器生成一个独一无二的闭包类. 而闭包中的语句
会变成闭包类的 operator() 函数具体执行指令. 那么我们如何通过程序证明确实生成了闭包类呢? 看下面代码:

```C++
namespace test_lambda_closure_class{
 int main(){
  auto lamb = [](){std::cout << __PRETTY_FUNCTION__ << ", "
                                    << __LINE__ << ", " << std::endl;};  // print the signature of the function and code line
  lamb();  // auto test_lambda_closure_class::main()::(anonymous class)::operator()() const, 115,
  return 0;
 }
}

int main(){
 test_lambda_closure_class::main();
 return 0;
}
```

从上面代码我们看到在 `lamb()` 输出了函数签名,看出来生成了一个匿名类,并且调用了其中的 `operator() const` 函数,
其中 lambda 表达式的内容是 `operator() const` 的具体执行内容.

同时闭包还支持下面的闭包复制操作:

```C++
namespace test_lambda_copy{
 int main(){
  int x = 0;
  auto lamb1 = [&x](){std::cout << __PRETTY_FUNCTION__ << ", "
                            << __LINE__ << ", " << "x = " << x++ << std::endl;};
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
```

在某些情况下, lambda 表达式、闭包和闭包类之间的都不区分.但是我们应该清楚的知道,这三个概念中:

* lambda 表达式和闭包类存在于编译期
* lambda closure 存在于运行期

**这里有个问题: 我们能够将下列代码改写成函数调用?**

```C++
namespace test_function_template_lambda{
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

int main(){
 test_function_template_lambda::main();
 return 0;
```

从上述代码的输出中我们看到, (1) (2) (3) 三种方法的输出结果相同,从 C++ 实现上讲 functor 和 lambda 表达式
等价,因此,这也提出了很重要的一点, 对于 lambda 表达式:

```C++
[](auto&& i){ std::cout << i << std::endl;};
```

中的 `auto&& i` 到底是**万能引用**还是**右值引用**? 答案是万能引用,因为如果我们将上述代码写成 functor,那么会涉及到
类型推导,并且其参数形式完全符合万能引用的要求,这一点一定要注意,**它是万能引用,并不是右值引用**. 这里的特别说明对
下一节中 lambda 形参有重要意义.

#### lambda 设计中需要思考的问题

##### lambda introducer capture

[Lambda introduer capture](https://github.com/abcdabcd3899/Dive-Into-Modern-CPP/blob/main/imgs/lambda_introducer.png)

C++ 14 增加了 init capture 的概念,所以我们可以在 [] 处用任何方法定义**匿名闭包类的成员函数**,
这极大丰富了 C++ lambda 的语义,测试代码如下:

```C++
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
```

##### lambda parameters

没有什么可多说的,就是普通函数的参数而已.

##### std::bind 与 lambda 使用场景探讨

尽量少用 std::bind. 因为 std::bind 实际在调用者和被调用者之间增加了一层,
这一中间层使得参数的传递发生了变化.

* 如果要传入引用,需要 使用 std::ref;
* 如果要传入 const 常量则需要 std::cref,这不友好.

```C++
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
 std::cout << "----- pass by value -----" << std::endl;
 test_bind_object_pass_by_value::main();
 std::cout << "----- pass by reference -----" << std::endl;
 test_bind_object_pass_by_ref::main();
 return 0;
}

```

从上面例子的输出我们得出:

* 在 `std::bind` 中想要使用引用方式传递参数,必须通过 `std::ref` 或 `std:cref` 两种
方式,否则就以默认 copy 的方式传递参数.
* std::bind 参数传递过程中,到底是 pass-by-value 还是 pass-by-reference 都不是那么明显,
需要调用方特别的小心.
* 加上 std namespace placeholder 之后的结果更加扑朔迷离.

根据官方文档, 从 C++ 14 开始, `std::bind` 已经彻底失去了用武之地.
