#pragma once
#include <thread>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <future>


#include <cstdio>
class ParallelProcessor
{
public:
	ParallelProcessor(int n_thread): 
		n_working(0), 
		ranges(n_thread, {0, 0}), 
		start(nullptr), 
		notify_end_f(n_thread), 
		notify_end_p(n_thread), 
		notify_end_cv(n_thread),
		is_terminate(false)
	{
		thread_pool.reserve(n_thread);
		for (int i = 0; i < n_thread; ++i)
		{
			thread_pool.emplace_back(
				[this](int i)
				{
					std::mutex m;
					std::unique_lock<std::mutex> lock(m);
					while (true)
					{
						work_blocker.wait(lock);
						if (is_terminate)
							break;
						if (last_worker <= i) 
							continue;
						for (size_t k = ranges[i].first; k < ranges[i].second; ++k)
						{
							func(start, k);
						}
						notify_end_p[i].set_value();
					}
				}, i
			).detach();
		}
	}

	template <typename Ptr>
	requires std::is_pointer_v<Ptr>
	void work(Ptr beg, size_t size, int n, std::function<void(void* start, size_t index)> func)
	{
		this->func = func;
		start = beg;
		int segment_size = size % n ? size / n + 1 : size / n;
		size_t left = 0;
		int count = 0;
		last_worker = 0;
		while (left < size)
		{
			size_t right = std::min(left + segment_size, size);
			ranges[last_worker] = {left, right};
			left = right;
			notify_end_p[last_worker] = std::move(std::promise<void>());
			notify_end_f[last_worker] = std::move(std::future<void>());
			notify_end_f[last_worker] = notify_end_p[last_worker].get_future();
			++last_worker;
		}
		n_working = last_worker;
		work_blocker.notify_all();

		for (int i = 0; i < last_worker; ++i)
		{
			notify_end_f[i].get();
		}
	}

private:
	std::vector<std::future<void>> notify_end_f;
	std::vector<std::promise<void>> notify_end_p;
	std::vector<std::condition_variable> notify_end_cv;
	std::condition_variable work_blocker;
	std::vector<std::pair<size_t, size_t>> ranges;
	// std::mutex m;
	std::function<void(void* start, size_t index)> func;
	void* start;
	std::atomic_int n_working;
	int last_worker;
	std::vector<std::thread> thread_pool;
	bool is_terminate;
};








// #pragma once
// #include <thread>
// #include <future>
// #include <functional>
// #include <type_traits>
// #include <algorithm>
// #include <mutex>


// // #include <iostream>
// // template <typename Iter>
// // concept RandomAccessIterator = 
// // 	std::is_same_v<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>;


// // template <typename Iter, typename F, typename ...ARG> 
// // requires RandomAccessIterator<Iter> || std::is_pointer_v<Iter>
// // void parallel(Iter beg, Iter end, int n, F f, ARG...arg)
// // {
// // 	size_t size = end - beg;
// // 	// n > 2;
// // 	size_t segment = size / n;
// // 	Iter left = beg;
// // 	Iter right;
// // 	std::vector<std::thread> worker;
// // 	worker.reserve(n + 1);
// // 	while (left < end)
// // 	{
// // 		Iter right = left + segment;
// // 		right = right > end ? end : right;
// // 		worker.emplace_back([left, right, &f, &arg...](){
// // 			for (Iter i = left; i < right; ++i)
// // 			{
// // 				f(i, arg...);
// // 			}
// // 		});
// // 		left = right;
// // 	}
// // 	for (auto& t: worker)
// // 	{
// // 		t.join();
// // 	}
// // }

// class ParallelProcessor
// {
// public:
// 	ParallelProcessor(int max_size):
// 		blockers(max_size),
// 		releasers(max_size),
// 		catchers(max_size),
// 		r_catchers(max_size),
// 		start(nullptr)
// 	{
// 		workers.reserve(max_size);
// 		for (int i = 0; i < max_size; ++i)
// 		{
// 			blockers[i] = std::move(releasers[i].get_future());
			
// 			workers.emplace_back(
// 				[this, i]()
// 				{
// 					while (true)
// 					{
// 						auto range = blockers[i].get();
// 						if (is_terminate) return;
// 						for (int k = range.first; k < range.second; ++k)
// 						{
// 							function(start, k);
// 						}
// 						releasers[i] = std::move(std::promise<std::pair<size_t, size_t>>());
// 						blockers[i] = std::move(releasers[i].get_future());
// 						r_catchers[i].set_value();
// 					}
// 				}
// 			).detach();
// 		}
// 	}

// 	~ParallelProcessor()
// 	{
// 		is_terminate = true;
// 		for (auto& r: releasers)
// 		{
// 			r.set_value({0, 0});
// 		}
// 	}

// 	template <typename Ptr>
// 	requires std::is_pointer_v<Ptr>
// 	void work(Ptr beg, size_t size, int n, std::function<void(void*, size_t)> func)
// 	{
// 		function = func;
// 		start = beg;
// 		int segment_size = size % n ? size / n + 1 : size / n;
// 		size_t left = 0;
// 		int count = 0;
// 		while (left < size)
// 		{
// 			size_t right = std::min(left + segment_size, size);
// 			r_catchers[count] = std::move(std::promise<void>());
// 			catchers[count] = std::move(r_catchers[count].get_future());
// 			releasers[count].set_value({left, right});
// 			left = right;
// 			++count;
// 		}
// 		for (int j = 0; j < count; ++j)
// 		{
// 			catchers[j].get();
// 		}
// 	}

// private:
// 	std::vector<std::thread> workers;
// 	std::vector<std::future<std::pair<size_t, size_t>>> blockers;
// 	std::vector<std::promise<std::pair<size_t, size_t>>> releasers;
// 	std::vector<std::future<void>> catchers;
// 	std::vector<std::promise<void>> r_catchers;
// 	bool is_terminate = false;

// 	std::function<void(void*, size_t)> function;
// 	void* start;
// };