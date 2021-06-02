#include "Timer.h"

Timer::Timer() {
	timer_start = std::chrono::system_clock::now();
}
Timer::~Timer() {

}

void Timer::ResetTimer()
{
	timer_start = std::chrono::system_clock::now();

}

unsigned int Timer::GetDuration()
{
	timer_end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli>duration = timer_end - timer_start;

	return duration.count() * std::chrono::milliseconds::period::num / std::chrono::milliseconds::period::den;
}
float Timer::GetMilisDuration()
{
	timer_end = std::chrono::system_clock::now();
	std::chrono::duration<float, std::milli>duration = timer_end - timer_start;

	return duration.count();
}