#pragma once
#include <chrono>

using namespace std::chrono_literals;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;