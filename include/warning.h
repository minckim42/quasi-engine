#pragma once
#include <iostream>

template <typename T>
void print_cerr(T t)
{
	std::cerr << t;
}

template <typename T, typename ...ARG>
void print_cerr(T t, ARG...arg)
{
	std::cerr << t;
	print_cerr(arg...);
}

template <typename ...ARG>
void WARNING(const std::string_view& func, ARG...arg)
{
	std::cerr << "[WARNING] " << func << ": ";
	print_cerr(arg...);
	std::cerr << '\n';
}