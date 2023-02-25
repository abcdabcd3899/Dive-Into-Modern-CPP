#include <iostream>
#include <optional>
#include <string>

namespace test_optional{
	std::optional<std::string> findItem(std::string str){
		if(str == "hello"){
			return "hello";
		}
		return std::nullopt;
	}

	int main(){
		std::optional<std::string> str1 = findItem("hello");
		if(str1) std::cout << *str1 << " " << str1->size() << std::endl;  // https://zh.cppreference.com/w/cpp/utility/optional/operator*
		std::optional<std::string> str2 = findItem("");
		if(!str2)std::cout << "input empty string" << std::endl;
		return 0;
	}
}

int main(){
	test_optional::main();
}