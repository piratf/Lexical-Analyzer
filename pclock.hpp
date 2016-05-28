#ifndef PCLOCK_H_
#define PCLOCK_H_

#include <cstdio>
#include <chrono>

namespace piratf {
    class Clock {
      public:
        Clock() = default;
        ~Clock() = default;

        void start(const char *message) {
            printf("%s\n", message);
            fflush(stdout);
            _begin = std::chrono::high_resolution_clock::now();
        }

        void start() {
            _begin = std::chrono::high_resolution_clock::now();
        }

        void terminal(const char *message) {
            printf("%s\n", message);
            fflush(stdout);
            _end = std::chrono::high_resolution_clock::now();
        }

        void terminal() {
            _end = std::chrono::high_resolution_clock::now();
        }

        void display_nano_seconds() {
            printf("%lld nano seconds\n", std::chrono::duration_cast<std::chrono::nanoseconds>(_end - _begin).count());
        }

        void display_micro_seconds() {
            printf("%lld micro seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(_end - _begin).count());
        }

        void display_milli_seconds() {
            printf("%lld milli seconds\n", std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count());
        }

        void display_seconds() {
            printf("%lld seconds\n", std::chrono::duration_cast<std::chrono::seconds>(_end - _begin).count());
        }

      private:
        decltype(std::chrono::high_resolution_clock::now()) _begin;
        decltype(std::chrono::high_resolution_clock::now()) _end;
    };
}

#endif