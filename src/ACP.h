#ifndef _ACP_H
#define _ACP_H

class ACP
{
    public:
        ACP();
		void drawAxis(Mat&, Point, Point, Scalar, const float);
		double getOrientation(const vector<Point> &, Mat&);
		void test();

};
#endif
