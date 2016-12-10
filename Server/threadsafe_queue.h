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

		class queue_quit : public std::exception
		{
		public:
			queue_quit() : std::exception() {}
		};

		void put(const T & value)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_queue.push(value);
			_cv.notify_one();
		}

		void get(T & dest) // blocking (cancel using quit())
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_cv.wait(lock, [this] { return !_queue.empty() || _quit; });
			if (_quit) throw queue_quit();
			dest = _queue.front();
			_queue.pop();
		}

		T get() // blocking (cancel using quit())
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_cv.wait(lock, [this] { return !_queue.empty() || _quit; });
			if (_quit) throw queue_quit();
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

		void quit()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_quit = true;
			_cv.notify_all();
		}

	protected:
		std::mutex _mutex;
		std::condition_variable _cv;
		std::queue<T> _queue;

		bool _quit = false;
	};
}
