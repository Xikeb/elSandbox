#pragma once
#include <array>
#include <atomic>
#include <functional>
#include <new> // For hardward_destructive_interference
#include <utility>
#include <vector>

namespace ecs {
    struct Job {
        using Function = std::function<void(Job const &)>;

        // Cannot construct by copy because missing predecessor to
        // update this, and indeterminate number of successors to allocate and copy
        Job(Job const &oth) = delete;
        Job(Job &&oth):
            predecessors(oth.predecessors.load()),
            action(std::move(oth.action)),
            successors(std::move(oth.successors))
            {
            }


        Job(std::nullptr_t = nullptr) noexcept: action(nullptr) {}
        Job(Function const &f): action(f) {}
        Job(Function &&f): action(std::move(f)) {}
        Job(std::function<void(void)> const &f): action([&f](Job const &){f();}) {}
        Job(std::function<void(void)> &&f): action([fun = std::move(f)](Job const &){fun();}) {}

        /*template<typename F>
          Job(F &&f): action(std::forward<F>(f)) {}*/

        void operator()() {
            this->action(*this);
            std::for_each(
                this->successors.begin(),
                this->successors.end(),
                [](Job *j) {--j->predecessors;}
                );
        }

        //Same as construction by copy, cannot.
        Job &operator=(Job const &oth) = delete;
        Job &operator=(Job &&oth) {
            this->predecessors = oth.predecessors.load();
            this->action = std::move(oth.action);
            this->successors = std::move(oth.successors);
            return *this;
        }

        std::atomic<std::size_t> predecessors{0};
        Function action;
        std::vector<Job *> successors;
        static constexpr std::size_t jobContentSize = sizeof(predecessors)
                                                  + sizeof(action)
                                                  + sizeof(successors);
        static constexpr std::size_t maxPaddingSize = 64 /*std::hardware_destructive_interference_size*/;
        static constexpr std::size_t jobPaddingSize = maxPaddingSize - (jobContentSize % maxPaddingSize);
        std::array<char, jobPaddingSize> padding;
    };
} // ecs