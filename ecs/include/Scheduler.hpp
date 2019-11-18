#pragma once
#include <cstdlib>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <deque>
#include <condition_variable>
#include <iostream>
#include <utility>

#include "Job.hpp"
#include "Worker.hpp"

namespace ecs {
    template<typename WorkerType>
    class PrimitiveScheduler {
    public:
        using Worker = WorkerType;
        using Task = WorkerType::Work;

        explicit PrimitiveScheduler(std::size_t workerCount = std::thread::hardware_concurrency() - 1):
            _workerCount{workerCount}, _workers{workerCount}
            {
                this->_threadPool.reserve(workerCount);

                for (auto i = workerCount; i; --i)
                    this->_threadPool.emplace_back([this, idx = i-1]{ this->run(idx); });
            }

        ~PrimitiveScheduler() {
            if (!this->_done) this->stop();

            std::for_each(
                this->_threadPool.begin(), this->_threadPool.end(),
                [](auto &&thr) { if (thr.joinable()) thr.join(); }
                );
        }

        void schedule(Task &&j) { //Add job to job list(s)
            if (this->_done) return;

            auto count = this->_workerCount;
            auto idx = this->_index++;

            for (std::size_t i = 0; i < count; ++i) {
                auto &worker = this->_workers[(idx + i) % count];
                if (worker.tryPush(std::move(j))) return;
            }

            this->_workers[idx % count].push(std::move(j));
        }

        template<typename Iter>
        void schedule(Iter first, Iter last) {
            if (this->_done) return;

            auto dist = std::distance(first, last);

            if (!dist) return;

            auto count = this->_workerCount;
            auto stride = (dist / count) ?: 1;
            auto next = first + stride;
            auto idx = this->_index++;

            for (std::size_t i = count; i; --i) {
                auto &worker = this->_workers[(idx + i) % count];
                if (worker.tryPush(first, next)) {
                    first = next;
                    next = (i <= 1) ? last : (next + stride);
                }
            }

            if (first >= last) {
                this->_workers[idx % count].push(first, last);
            }
        }

        template<typename ...Args>
        void schedule_inplace(Args&&... args) { //Add job to job list(s)
            if (this->_done) return;

            auto count = this->_workerCount;
            auto idx = this->_index++;

            for (std::size_t i = 0; i < count; ++i) {
                auto &worker = this->_workers[(idx + i) % count];
                if (worker.tryEmplace(std::forward<Args>(args)...)) return;
            }

            this->_workers[idx % count].emplace(std::forward<Args>(args)...);
        }

        void stop() {
            this->_done = true;
            std::for_each(
                std::begin(this->_workers),
                std::end(this->_workers),
                [](auto &w) { w.stop(); }
                );
        }

        Worker &anyWorker() {
            size_t idx = this->_index;
            size_t count = this->_workerCount;
            size_t offset;

            do {
                offset = (idx + std::rand()) % count;
            } while (offset == idx);

            return this->_workers[offset];
        }

    private:
        std::atomic<std::size_t> _index = 0;
        size_t _workerCount;
        std::vector<Worker> _workers;
        std::vector<std::thread> _threadPool;
        bool _done = false;

        void run(std::size_t idx) {
            Job j(nullptr);

            while (!this->_done) {
                if (this->getWork(j, idx))
                    j();
            }
        }

        bool getWork(Task &j, std::size_t idx) {
            if (this->_done) return false;

            if (this->_workers[idx].tryPop(j)) {
                return true;
            }
            
            auto count = this->_workerCount;
            for (std::size_t i = count; i > 1; --i) {
                auto &worker = this->_workers[(idx + i) % count];
                if (worker.trySteal(j)) {
                    //if (i) std::cout << "steal" << std::endl;
                    return true;
                }
            }

            return this->_workers[idx % count].pop(j);
        }
    };
} // ecs
