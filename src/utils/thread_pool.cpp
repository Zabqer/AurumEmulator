#include "thread_pool.h"
#include "../log.h"
#include "../synchronized.h"

ThreadPool::ThreadPool() {
		resize(0);
}

ThreadPool::ThreadPool(size_t count) {
		resize(count);
}

ThreadPool::~ThreadPool() {
		resize(0);
}

void ThreadPool::resize(size_t count) {
		logC("ThreadPool::resize()");
		if (count < workers.size()) {
				synchronized(workers_mutex);
				while (count != workers.size()) {
				 		*workers[workers.size() - 1].exit = true;
						workers[workers.size() - 1].thread.join();
						delete workers[workers.size() - 1].exit;
						workers.erase(workers.end());
				}
		} else if (count > workers.size()) {
				synchronized(workers_mutex);
				for (unsigned int i = 0; i < count - workers.size(); i++) {
						bool* exit = new bool(false);
						workers.push_back({std::thread([this, exit] {
								Task task;
								while (!*exit) {
										while (true) {
												if (*exit) {
														return;
												}
												synchronized(tasks_mutex);
												std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
												for (int i = 0; i < tasks.size(); i++) {
														if (tasks[i].deadline <= now) {
																task = tasks[i];
																tasks.erase(tasks.begin() + i);
																goto run;
														}
												}
										}
										run:
										{
												task.function();
										}
								}
						}), exit});
				}
		}
}

void ThreadPool::post(TaskFunction function, std::chrono::milliseconds delay) {
		logC("ThreadPool::post()");
		synchronized(tasks_mutex);
		tasks.push_back({function, std::chrono::steady_clock::now() + delay});
}