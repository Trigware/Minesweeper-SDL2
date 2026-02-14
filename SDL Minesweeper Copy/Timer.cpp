#include "Timer.h"
#include "Globals.h"

void Timer::Progress() {
	if (paused) return;
	int stopwatchMultiplier = isStopwatch ? 1 : -1;
	time += deltaTime * stopwatchMultiplier;
	if (time > 0) return;

	endedOnLastUpdate = !timerEnded;
	timerEnded = true;
	time = 0;
	paused = true;
	if (functionOnTimeout != nullptr) functionOnTimeout();
}

Timer::Timer(float timerTime, bool pausedAtStart) : fullTime(timerTime), time(timerTime), paused(pausedAtStart) {
	isStopwatch = false;
	allTimers.push_back(this);
}

Timer::Timer(bool stopwatchPausedAtStart) {
	isStopwatch = true;
	paused = stopwatchPausedAtStart;
	allTimers.push_back(this);
}

Timer::~Timer() {
	std::erase(allTimers, this);
}

std::vector<Timer*> Timer::allTimers = {};

std::string Timer::GetTime() {
	int totalTimeInSec = time;
	std::string result = "";
	int hoursCount = totalTimeInSec / secondsInHour;
	int currentHourMinuteCount = (totalTimeInSec - hoursCount * secondsInHour) / secondsInMinute;
	int currentMinuteSecondCount = totalTimeInSec % secondsInMinute;

	if (hoursCount > 0) result += std::to_string(hoursCount) + ":";
	std::string minutesAsStr = std::to_string(currentHourMinuteCount);
	if (currentHourMinuteCount < 10 && hoursCount > 0) minutesAsStr = "0" + minutesAsStr;
	result += minutesAsStr + ":";
	std::string secondsAsStr = std::to_string(currentMinuteSecondCount);
	if (currentMinuteSecondCount < 10) secondsAsStr = "0" + secondsAsStr;
	result += secondsAsStr;

	return result;
}

float Timer::GetProgress() {
	if (reversedProgress) return time / fullTime;
	return 1.0 - time / fullTime;
}

Timer& Timer::operator=(const Timer& copy) {
	functionOnTimeout = copy.functionOnTimeout;
	fullTime = copy.fullTime; time = copy.time;
	paused = copy.paused; isStopwatch = copy.isStopwatch;
	return *this;
}

void Timer::Wait(float waitTime, const std::function<void()>& timeoutFunction) {
	Timer* waitTimer = new Timer(waitTime, false);
	waitTimer->functionOnTimeout = timeoutFunction;
}

void Timer::Reverse() {
	reversedProgress = !reversedProgress;
	float targetProgress = reversedProgress ? 0 : 1;
	paused = timerEnded = false;
	float progressBeforeReverse = GetProgress(), remainingProgress = 1 - std::abs(progressBeforeReverse - targetProgress);
	time = fullTime * remainingProgress;
}

float Timer::Lerp(float start, float end, LerpData data) { return Lerp::Get(start, end, GetProgress(), data); }