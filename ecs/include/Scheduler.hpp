#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <deque>
#include <condition_variable>

namespace ecs {
	class Scheduler {
	private:
		using Task = std::function<void(void)>;

		class TaskQueue {
		public:
			void push(Task const &t)
			{
				std::lock_guard l{this->m};
				this->queue.push_back(t);
				this->count.fetch_add(1);
			}

			void push(Task &&t)
			{
				std::lock_guard l{this->m};

				this->queue.push_back(std::move(t));
				this->count.fetch_add(1);
			}

			bool pop(Task &t)
			{
				std::lock_guard l{this->m};

				if (!this->count)
					return false;

				this->count.fetch_sub(1);
				t = std::move(this->queue.front());
				this->queue.pop_front();
				return true;
			}

			std::atomic<size_t> count{0};
			std::deque<Task> queue{256};
			std::mutex m{};
		};

		class Worker {
		public:
			Worker(std::size_t id):
				id{id},
				thr{[this]{ this->run(); }}
			{
			}

			void run()
			{

			}

			size_t id;
			std::thread thr;
		};
	public:
		explicit Scheduler(size_t poolSize = 4):
			tasks{poolSize},
			threadPool{poolSize},
			running{true}
		{
		}

		void schedule(Task const &t) { this->tasks.push(t); this->taskAvailable.notify_one(); }
		void schedule(Task &&t) { this->tasks.push(t); this->taskAvailable.notify_one(); }

		void work(size_t /*myId*/ = 0)
		{
			Task t;

			while (this->running) {
				while (!this->tasks.pop(t)) {
					std::unique_lock lk{this->m};
					this->taskAvailable.wait(lk, [this]{ return this->tasks.count == 0u; });
				}
				t();
			}
		}

		std::mutex m;
		std::condition_variable taskAvailable;
		std::unique_lock<std::mutex> ul{m, std::defer_lock};
		TaskQueue tasks;
		std::vector<std::thread> threadPool;
		bool running:1;
	};
} // ecs