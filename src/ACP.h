#ifndef _ACP_H
#define _ACP_H

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class ACP
{
    public:
        ACP();
		void drawAxis(Mat&, Point, Point, Scalar, const float);
		double getOrientation(const vector<Point> &, Mat&);
		void test();

};
#endif
