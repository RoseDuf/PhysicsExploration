#include "Gravity.h"

float Gravity::calculateVelocity(float u, float t)
{
	return u + marsGravitationalConstant * t;
}

float Gravity::calculatePositionFromVelocity(float s0, float u, float t)
{
	return s0 + u * t + (1 / 2.0f) * marsGravitationalConstant * t * t;
}