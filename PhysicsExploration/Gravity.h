#pragma once
/*
	Knowns per frame
	u = current velocity
	t = deltaTime or time difference
	s0 = current position of particle or object ( centre of mass generally )
	g = gravitational acceleration of any planet but can be tweaked

	Unknowns to be computed at every frame
	v = velocity for next time frame
	v = u + g * t

	s = next position of centre of mass an object
	s = s0 + u * t + 1/2 * g * t^2
*/
class Gravity
{
private:
	const float EARTH_GRAVITATIONAL_ACCELERATION = 9.807;
	const float MARS_GRAVITATIONAL_ACCELERATION = 3.721;
public:
	float calculateVelocity(float u, float t);
	float calculatePositionFromVelocity(float s0, float u, float t);
};

