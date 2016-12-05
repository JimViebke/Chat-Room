#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

/*
Jim Viebke
A basic thread-safe wrapper around a std::queue
*/

namespace threadsafe
{
	template<typename T>
	class queue
	{
	public:
		queue() {}

		void put(const T & value)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.push(value);
			_cv.notify_one();
		}

		void get(T & dest) // blocking
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_cv.wait(lock, [this] { return !_queue.empty(); });
			dest = _queue.front();
			_queue.pop();
		}

		T get() // blocking
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_cv.wait(lock, [this] { return !_queue.empty(); });
			T data = _queue.front();
			_queue.pop();
			return data;
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _queue.empty();
		}

		size_t size() const
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _queue.size();
		}

	protected:
		std::mutex _mutex;
		std::condition_variable _cv;
		std::queue<T> _queue;
	};
}
