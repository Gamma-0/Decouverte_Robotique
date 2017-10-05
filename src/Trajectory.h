#ifndef _TRAJECTORY_H
#define _TRAJECTORY_H

#include "ImageProcessing.h"

class Trajectory
{
public:
	double d; /* Average speed of robot */
	double D; /* Space between to wheels */
	double R; /* Radius of wheels */

public:
	explicit Trajectory();
	double computeAngle(const Axis &axis);
	double computeAngularSpeed(double angle, long time);
	void motorsSpeed(double alpha, double &left, double &right);

};

#endif// _TRAJECTORY_H
