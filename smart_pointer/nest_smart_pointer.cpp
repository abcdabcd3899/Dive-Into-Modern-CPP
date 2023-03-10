#include <memory>
#include <iostream>

struct Node
{
	int data;
	// std::unique_ptr<Node> next;  // 这种写法会导致 stack overflow
	Node* next;  // 修改成裸指针，然后增加下面的构造函数，整个程序运行正常了

	Node(int _data, Node* next = nullptr):data{_data}{};

	~Node()
	{
		std::cout<<"dtor: "<<data<<std::endl;
	}
};

struct List
{
	std::unique_ptr<Node> head;
	void push(int data)
	{
		head = std::unique_ptr<Node>(new Node{data, head.get()});
	}
};

int main()
{
	List wall;
	for (int beer = 0; beer != 30000000; ++beer)
	{
		wall.push(beer*10);
	}
}

// #include <memory>
// #include <iostream>
// using namespace std;

// struct Node
// {
//     int data;
//     std::unique_ptr<Node> next;

//     ~Node()
//     {
//         cout<<"dtor: "<<data<<endl;
//     }
// };

// struct List
// {
// 	std::unique_ptr<Node> head;

// 	void push(int data)
// 	{
// 		head = std::unique_ptr<Node>(new Node{data, std::move(head)});
// 	}

// 	~List(){  // 如果不写这里，存在递归析构的问题
// 		while(head){
// 			head = std::move(head->next);
// 		}
// 	}
// };

// int main()
// {
// 	List wall;
// 	for (int beer = 0; beer != 300000000; ++beer)
// 	{
// 		wall.push(beer*10);
// 	}
// }
