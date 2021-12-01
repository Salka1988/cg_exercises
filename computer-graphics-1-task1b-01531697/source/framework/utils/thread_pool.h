


#ifndef INCLUDED_FRAMEWORK_UTILS_THREAD_POOL
#define INCLUDED_FRAMEWORK_UTILS_THREAD_POOL

#pragma once

#include <cstddef>
#include <vector>
#include <atomic>
#include <thread>
#include <functional>


class ThreadPool
{
	std::vector<std::thread> threads;

public:
	ThreadPool() = default;
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator =(const ThreadPool&) = delete;

	template <typename... Args>
	std::thread& launch(Args&&... args)
	{
		return threads.emplace_back(std::forward<Args>(args)...);
	}

	~ThreadPool()
	{
		for (auto& t : threads)
			t.join();
	}
};

constexpr std::size_t divup(std::size_t a, std::size_t b)
{
	return (a + b - 1) / b;
}

template <typename F>
void renderthread(F& f, int width, int height, int tile_width, int tile_height, std::atomic<int>& next, int thread_id)
{
	const int W = static_cast<int>(divup(width, tile_width));
	const int H = static_cast<int>(divup(height, tile_height));
	const int N = W * H;

	while (true)
	{
		int n = next.fetch_add(1, std::memory_order_relaxed);

		int j = n / W;

		if (j >= H)
			return;

		int i = n % W;

		int left = i * tile_width;
		int top = j * tile_height;
		int right = std::min(left + tile_width, width);
		int bottom = std::min(top + tile_height, height);

		f(left, top, right, bottom, n, N, thread_id);
	}
}

template <typename F>
void launchTiled(F&& f, int num_threads, int width, int height, int tile_width, int tile_height)
{
	ThreadPool pool;

	std::atomic<int> next = 0;

	for (int i = 1; i < num_threads; ++i)
		pool.launch(std::ref(renderthread<F>), std::ref(f), width, height, tile_width, tile_height, std::ref(next), i);
	renderthread<F>(f, width, height, tile_width, tile_height, next, 0);
}

#endif  // INCLUDED_FRAMEWORK_UTILS_THREAD_POOL
