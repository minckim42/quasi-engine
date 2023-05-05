#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <vector>

#include <cstdio>

struct Bar
{
	Bar(int k)
	{
		a[0] = k;
	}
	int v[10000000];
};

struct Foo
{
	std::shared_ptr<Bar> v;
	Foo(int a): v(std::make_shared<Bar>(a)) {}
};


int main()
{
	std::vector<Foo> con;

	std::mutex m;

	std::thread t0(
		[&]() {
			int k = 0;
			while (true)
			{
				for (int i = 0; i < 10; i++)
				{
					std::lock_guard<std::mutex> g(m);
					con.emplace_back(i);
				}
				{
					std::lock_guard<std::mutex> g(m);
					con.clear();
				}
			}
		}
	);

	std::thread t1(
		[&]() {
			while (true)
			{

				std::vector<Foo> cop;
				{
					std::lock_guard<std::mutex> g(m);
					cop = con;
				}
				for (Foo& i: cop)
				{
					std::cout << i.v->v[0] << std::endl;
				}
				std::cout << "========\n";
				{
					std::lock_guard<std::mutex> g(m);
					cop.clear();
				}
			}
		}
	);
	t0.join();
	t1.join();
}