#pragma once
#include "UDPSocket.h"	
class Timer
{
private:
	std::chrono::system_clock::time_point timer_start;
	std::chrono::system_clock::time_point timer_end;

public:
	Timer();
	~Timer();

	void ResetTimer();
	unsigned int GetDuration();
	float GetMilisDuration();
};