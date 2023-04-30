#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>

class Foo
{
public:
	template <typename...Arg>
	Foo(Arg...arg)
	{
		load(arg...);
		std::cout << "BYE\n";
	}

	template <typename T, typename...Arg>
	void load(T t, Arg...arg)
	{
		std::cout << t << std::endl;
		load(arg...);
	}

	void load()
	{
		std::cout << "end?" << std::endl;
	}

	Foo(Foo&& other) noexcept
	{
		std::cout << "move construct\n";
	}
	
	Foo()
	{
		std::cout << "default\n";
	}

	Foo& operator=(Foo&& other) noexcept
	{
		std::cout << "move assign\n";
		return *this;
	}
};

std::map<int, Foo> m;

int main()
{
	m[15] = Foo(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	// m.emplace(15, Foo{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
}