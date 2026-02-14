#pragma once

enum class TransitionType {
	Power,
	Sine
};

struct Transition {
	Transition(float degree) : powerDegree(degree) {}
	Transition(TransitionType type) : transType(type) {}
	float powerDegree = 1;
	TransitionType transType = TransitionType::Power;
};

enum class EaseType {
	In,
	Out,
	InOut
};

struct LerpData {
	Transition transition;
	EaseType easeType;
	LerpData(Transition trans, EaseType ease) : transition(trans), easeType(ease) {}
	static const LerpData InOutQuad, Linear;
};

class Lerp {
public:
	static float Get(float start, float end, float progress, LerpData data = LerpData::Linear);
private:
	static float TransformProgress(float progress, Transition transition, EaseType easeType);
	static float GetPowerProgress(float progress, Transition transition, EaseType easeType);
};