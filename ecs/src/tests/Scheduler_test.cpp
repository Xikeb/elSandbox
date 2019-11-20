#include <iostream>
#include <string>
#include <chrono>
#include <cstring>
#include "Scheduler.hpp"
#include "el/detail/pretty_print.hpp"

using namespace std;
using namespace std::chrono;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

struct Timer {
    Timer(Duration &p): store(p), start(Clock::now()) {}
    ~Timer()
    {
        auto end = Clock::now();
        store = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    }

    Duration &store;
    TimePoint start;
};

std::size_t prime_generator(std::size_t max) {
	std::size_t primes[max];
	std::size_t count = 0;
	std::size_t k = 1;
	bool isprime;
	
	std::memset(primes, 0, sizeof(std::size_t) * max);
	while (count < max) {
		++k;
		
		isprime = true;
		for (auto i = 0; isprime && (i < count); ++i)
			isprime &= (k % primes[i] != 0);
		
		if (isprime) primes[count++] = k;
	}
	
	return k;
}

#define REPS 10
#define COUNT 10'000'000

int main()
{
	cout << boolalpha;
	Duration durSchd, durSerial;

	{
		ecs::PrimitiveScheduler schd;
        Timer t{durSchd};
        for (auto i = COUNT; i; --i) {
			/*std::size_t max = random() % 250;
			schd.schedule([max](auto &&){ prime_generator(max); });
			schd.schedule([max](auto &&){ prime_generator(max); });
			schd.schedule([max](auto &&){ prime_generator(max); });
			schd.schedule([max](auto &&){ prime_generator(max); });*/

			schd.schedule_inplace(
				[i]() {
					int x;
					int reps = REPS + (REPS * (rand() % 5));
					for(int n = 0; n < reps; ++n)
						x = i + rand();
				});
		}
        std::cout << "done with the job posting" << std::endl;
    }
    cout << "schd\ttimer: \e[96m" << durSchd.count() / 1'000'000 << "\e[0mms" << endl;

    {
        ecs::Job j(nullptr);
        Timer t{durSerial};
		for (auto i = COUNT; i; --i) {
			([i](auto &&) {
                int x;
                int reps = REPS + (REPS * (rand() % 5));
                for(int n = 0; n < reps; ++n)
                    x = i + rand();
            })(j);
		}
    }
    cout << "serial\ttimer: \e[96m" << durSerial.count() / 1'000'000 << "\e[0mms." << endl;

    return 0;
}
