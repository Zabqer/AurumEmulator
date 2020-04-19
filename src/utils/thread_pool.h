#ifndef __AURUM_THREAD_POOL_H__
#define __AURUM_THREAD_POOL_H__

#include <functional>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

class ThreadPool {
		public:
				typedef std::function<void()> TaskFunction;
		private:
				struct Task {
						TaskFunction function;
						std::chrono::steady_clock::time_point deadline;
				};
				struct Worker {
						std::thread thread;
						bool* exit;
				};
				std::vector<Worker> workers;
				std::mutex workers_mutex;
				std::vector<Task> tasks;
				std::mutex tasks_mutex;
		public:
				ThreadPool();
				ThreadPool(size_t);
				~ThreadPool();
				void post(TaskFunction, std::chrono::milliseconds = std::chrono::milliseconds(0) ) ;
				void resize(size_t);
};

#endif