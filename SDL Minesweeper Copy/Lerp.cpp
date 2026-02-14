#include "Lerp.h"
#include <cmath>
#include <iostream>

const LerpData LerpData::InOutQuad = LerpData(2, EaseType::InOut), LerpData::Linear = LerpData(1, EaseType::In);

float Lerp::Get(float start, float end, float progress, LerpData data) {
	float transformedProgress = TransformProgress(progress, data.transition, data.easeType);
	return start + (end - start) * transformedProgress;
}

// Formulas from easings.net
float Lerp::TransformProgress(float progress, Transition transition, EaseType easeType) {
	switch (transition.transType) {
		case TransitionType::Power: return GetPowerProgress(progress, transition, easeType);
	}
	return 1;
}

float Lerp::GetPowerProgress(float progress, Transition transition, EaseType easeType) {
	switch (easeType) {
		case EaseType::In: return std::pow(progress, transition.powerDegree);
		case EaseType::Out: return 1 - std::pow(1 - progress, transition.powerDegree);
		case EaseType::InOut:
			float firstPart = std::pow(2, transition.powerDegree - 1) * std::pow(progress, transition.powerDegree),
				secondPart = 1 - std::pow(-2 * progress + 2, transition.powerDegree) / 2;
			return progress <= 0.5 ? firstPart : secondPart;
	}
	return 1;
}