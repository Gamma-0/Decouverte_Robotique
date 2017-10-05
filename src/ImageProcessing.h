#ifndef _IMAGE_PROCESSING_H
#define _IMAGE_PROCESSING_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class Axis;

class ImageProcessing
{
	public:
		static void test();
		static Axis compute(
			const Mat &img,				/* Image to proceed */
			const Vec3b &color,			/* color to found in image */
			double colorThreshold,		/* threshold for color detection */
			double minArea);			/* area minimum for color regions */

		static Mat makeMedianKernel(unsigned size);

	private:
		explicit ImageProcessing();

		static void cleanImage(const Mat &img, Mat &out);
		static void extractColorAsBlackWhite(
			const Mat &img,
			Mat &out,
			const Vec3b &color,
			double colorThreshold);
		static vector<vector<Point> > getRegions(Mat &img);
		static void performRegionPCA(const vector<Point> &pts, Axis &axis);

		static double getOrientation(const vector<Point> &pts, Mat &img);
		static void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 0.2);
};

class Axis
{
public:
	double area;
	Point center, p1, p2;
	explicit Axis()
	{}
	explicit Axis(Point &center, Point &p1, Point &p2):
		center(center), p1(p1), p2(p2)
	{}
};

#endif //_IMAGE_PROCESSING_H
