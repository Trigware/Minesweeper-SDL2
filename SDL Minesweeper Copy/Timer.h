#pragma once
#include <vector>
#include <functional>
#include <string>
#include "Lerp.h"

class Timer {
public:
	static std::vector<Timer*> allTimers;
	static void Wait(float waitTime, const std::function<void()>& timeoutFunction);
	float fullTime = 0, time = 0;
	bool paused = true, isStopwatch = false, reversedProgress = false;
	std::function<void()> functionOnTimeout = nullptr;
	Timer(float timerTime, bool pausedAtStart);
	Timer(bool stopwatchPausedAtStart);
	Timer() = default;
	~Timer();

	bool Timeout() const { return time <= 0 && !endedOnLastUpdate; }
	void Restart() { time = fullTime; paused = false; timerEnded = false; }
	void Progress();
	void Unpause() { paused = false; }
	void Reverse();
	std::string GetTime();
	float GetProgress();
	Timer& operator=(const Timer& copy);
	float Lerp(float start, float end, LerpData data = LerpData::Linear);
private:
	const int secondsInHour = 3600, secondsInMinute = 60;
	bool endedOnLastUpdate = false, timerEnded = false;
};