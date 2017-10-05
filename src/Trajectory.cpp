#include "Trajectory.h"
#include <math.h>

Trajectory::Trajectory():
	d(0.26), D(125.0), R(31.5)
{}

double Trajectory::computeAngle(const Axis &axis)
{
	Point pMax;
	if (axis.p1Length() > axis.p2Length())
		pMax = axis.p1;
	else
		pMax = axis.p2;

	double tmp = axis.center.y - pMax.y;
	double angle = cos(
		norm(tmp) /
		sqrt(pow(tmp, 2.0) + pow(axis.center.x - pMax.x, 2.0))
	);
	return angle;
}

double Trajectory::computeAngularSpeed(double angle, long time)
{
	return angle * time;
}

void Trajectory::motorsSpeed(double alpha, double &left, double &right)
{
	left = ((2.0 * d) - (alpha * D)) / (4.0 * M_PI * R);
	right = ((2.0 * d) + (alpha * D)) / (4.0 * M_PI * R);
}
