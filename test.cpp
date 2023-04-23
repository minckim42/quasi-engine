#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

void foo(std::chrono::duration<long long, std::milli> d)
{
	std::cout << d.count() << std::endl;
}


int main()
{
using namespace std::chrono;

high_resolution_clock::time_point t0 = high_resolution_clock::now();
std::this_thread::sleep_for(1s);
high_resolution_clock::time_point t1 = high_resolution_clock::now();

nanoseconds d0 = t1 - t0;
microseconds d1 = std::chrono::duration_cast<microseconds>(d0);
duration<long long, std::milli> d2 = duration_cast<duration<long long, std::milli>>(d0);
auto d3 = duration_cast<duration<long long>>(d0);
auto d4 = duration_cast<duration<double>>(d0);

std::cout << "1 second-> nanoseconds        : " << d0.count() << std::endl;
std::cout << "1 second-> microseconds       : " << d1.count() << std::endl;
std::cout << "1 second-> milliseconds       : " << d2.count() << std::endl;
std::cout << "1 second-> seconds(long long) : " << d3.count() << std::endl;
std::cout << "1 second-> seconds(double)    : " << d4.count() << std::endl;

}