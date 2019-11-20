#pragma once

#include <algorithm>
#include <deque>
#include <condition_variable>
#include <mutex>

#include "Job.hpp"
#include "fwd/Scheduler.hpp"

namespace ecs {
    namespace impl {
    	template<typename TaskType, template<typename ...> class ContainerType>
    	struct SimpleWorker {
            using Self = SimpleWorker<TaskType, ContainerType>;
    	    using Task = TaskType;
    	    using TaskList = ContainerType<Task>;
            using Scheduler = ecs::PrimitiveScheduler<Self>;

            TaskList _taskList;
            std::mutex _mutex;
            std::condition_variable _cv;
            Scheduler *_schd;
            std::atomic _burden = 0;
            // std::thread _thr;
            bool _done = false;


            Worker() = default;
            explicit Worker(Scheduler &schd): _schd(schd)
            {
            }

            void stop() {
                this->_done = true;
                this->_cv.notify_all();
            }
            //Todo: make this a bounded SPMC queue
            //Todo: use a Read-Write lock system
            //Todo: maybe let this work as a circular buffer

            void push(Task &&j) {
                {
                    std::scoped_lock lk{this->_mutex};
                    this->_taskList.push_back(std::move(j));
                }

                ++this->_burden;
                this->_cv.notify_one();
            }

            template<typename Iter>
            void push(Iter first, Iter last) {
                auto itNewEnd, itLastEnd = std::end(this->_taskList);

                {
                    std::scoped_lock lk{this->_mutex};
                    itNewEnd = std::remove_copy(
                        std::make_move_iterator(first),
                        std::make_move_iterator(last),
                        itLastEnd, [](auto &&t) { return !t; }
                    );
                }

                this->_burden += std::distance(itLastEnd, itNewEnd);
                this->_cv.notify_all();
            }

            bool tryPush(Task &&j) {
                if (std::unique_lock lk{this->_mutex, std::try_to_lock}) {
                    this->_taskList.push_back(std::move(j));
                } else {
                    return false;
                }

                ++this->_burden;
                this->_cv.notify_one();
                return true;
            }

            template<typename Iter>
            bool tryPush(Iter first, Iter last) {
                auto itNewEnd, itLastEnd = std::end(this->_taskList);
                if (std::unique_lock lk{this->_mutex, std::try_to_lock}) {
                    itNewEnd = std::remove_copy(
                        std::make_move_iterator(first),
                        std::make_move_iterator(last),
                        std::end(this->_taskList),
                        itLastEnd, [](auto &&t) { return !t; }
                    );
                } else {
                    return true;
                }

                this->_burden += std::distance(itLastEnd, itNewEnd);
                this->_cv.notify_all();
                return true;
            }

            template<typename ...Args>
            void emplace(Args&&... args) {
                {
                    std::scoped_lock lk{this->_mutex};
                    this->_taskList.emplace_back(std::forward<Args>(args)...);
                }

                ++this->_burden;
                this->_cv.notify_one();
            }

            template <typename ...Args>
            bool tryEmplace(Args&&... args) {
                if (std::unique_lock lk{this->_mutex, std::try_to_lock}) {
                    this->_taskList.emplace_back(std::forward<Args>(args)...);
                } else {
                    return false;
                }

                ++this->_burden;
                this->_cv.notify_one();
                return true;
            }

            bool pop(Task &j) {
                {
                    std::unique_lock lk{this->_mutex};

                    this->_cv.wait(lk, [this]{ return this->_done || !this->_taskList.empty(); });
                    if (this->_taskList.empty()) return false;

                    j = std::move(this->_taskList.back());
                    this->_taskList.pop_back();
                }

                --this->_burden;
                return true;
            }

            bool tryPop(Task &j) {
                {
                    std::unique_lock lk{this->_mutex, std::try_to_lock};

                    if (!lk || this->_taskList.empty()) return false;

                    j = std::move(this->_taskList.back());
                    this->_taskList.pop_back();
                }

                --this->_burden;
                return true;
            }

            bool steal(Task &j) {
                {
                    std::unique_lock lk{this->_mutex};

                    this->_cv.wait(lk, [this]{ return this->_done || !this->_taskList.empty(); });
                    if (this->_taskList.empty()) return false;

                    j = std::move(this->_taskList.front());
                    this->_taskList.pop_front();
                }

                --this->_burden;
                return true;
            }

            bool trySteal(Task &j) {
                {
                    std::unique_lock lk{this->_mutex, std::try_to_lock};

                    if (!lk || this->_taskList.empty()) return false;

                    j = std::move(this->_taskList.front());
                    this->_taskList.pop_front();                    
                }

                --this->_burden;
                return true;
            }
        };
    } // impl
} // ecs
