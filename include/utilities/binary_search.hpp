#pragma once
#include <functional>
#include <type_traits>

template <typename Iter>
concept RandomAccessIterator = 
	std::is_same_v<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;

template <typename Iter, typename Tp, typename F> requires RandomAccessIterator<Iter>
Iter b_search(Iter begin, Iter end, const Tp& v, F less)
{
	if (begin + 1 == end || begin == end)
		return begin;

	Iter mid = begin + (end - begin) / 2;

	if (less(v, *mid))
	{
		return b_search<Iter, Tp, F>(begin, mid, v, less);
	}
	else
	{
		return b_search<Iter, Tp, F>(mid, end, v, less);
	}
}

template <typename Iter, typename Tp> requires RandomAccessIterator<Iter>
Iter b_search(Iter begin, Iter end, const Tp& v)
{
	if (begin + 1 == end || begin == end)
		return begin;

	Iter mid = begin + (end - begin) / 2;

	if (v < *mid)
	{
		return b_search<Iter, Tp>(begin, mid, v);
	}
	else
	{
		return b_search<Iter, Tp>(mid, end, v);
	}
}