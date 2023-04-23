#pragma once
#include <thread>
#include <future>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <mutex>


#include <iostream>
template <typename Iter>
concept RandomAccessIterator = 
	std::is_same_v<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;


template <typename Iter, typename F, typename ...ARG> 
requires RandomAccessIterator<Iter> || std::is_pointer_v<Iter>
void parallel(Iter beg, Iter end, int n, F f, ARG...arg)
{
	size_t size = end - beg;
	// n > 2;
	size_t segment = size / n;
	Iter left = beg;
	Iter right;
	std::vector<std::thread> worker;
	worker.reserve(n + 1);
	while (left < end)
	{
		Iter right = left + segment;
		right = right > end ? end : right;
		worker.emplace_back([left, right, &f, &arg...](){
			for (Iter i = left; i < right; ++i)
			{
				f(i, arg...);
			}
		});
		left = right;
	}
	for (auto& t: worker)
	{
		t.join();
	}
}

#include <mutex>

class ParallelProccessor
{
public:
	ParallelProccessor(int max_size):
		blockers(max_size),
		releasers(max_size),
		catchers(max_size),
		r_catchers(max_size)
	{
		workers.reserve(max_size);
		for (int i = 0; i < max_size; ++i)
		{
			blockers[i] = std::move(releasers[i].get_future());
			
			workers.emplace_back(
				[this, i]()
				{
					while (true)
					{
						auto range = blockers[i].get();
						if (terminate) return;
						for (int k = range.first; k < range.second; ++k)
						{
							function(start, k);
						}
						releasers[i] = std::move(std::promise<std::pair<size_t, size_t>>());
						blockers[i] = std::move(releasers[i].get_future());
						r_catchers[i].set_value();
					}
				}
			).detach();
		}
	}

	~ParallelProccessor()
	{
		terminate = true;
		for (auto& r: releasers)
		{
			r.set_value({0, 0});
		}
	}

	template <typename Ptr>
	requires std::is_pointer_v<Ptr>
	void work(Ptr beg, size_t size, int n, std::function<void(void*, size_t)> func)
	{
		function = func;
		start = beg;
		int segment_size = size % n ? size / n + 1 : size / n;
		size_t left = 0;
		int count = 0;
		while (left < size)
		{
			size_t right = std::min(left + segment_size, size);
			r_catchers[count] = std::move(std::promise<void>());
			catchers[count] = std::move(r_catchers[count].get_future());
			releasers[count].set_value({left, right});
			left = right;
			++count;
		}
		for (int j = 0; j < count; ++j)
		{
			catchers[j].get();
		}
	}

private:
	std::vector<std::thread> workers;
	std::vector<std::future<std::pair<size_t, size_t>>> blockers;
	std::vector<std::promise<std::pair<size_t, size_t>>> releasers;
	std::vector<std::future<void>> catchers;
	std::vector<std::promise<void>> r_catchers;
	bool terminate = false;

	std::function<void(void*, size_t)> function;
	void* start;
};