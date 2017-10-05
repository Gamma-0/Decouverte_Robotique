#include <iostream>
#include <stdio.h>
#include <math.h>
#include "Robot.h"
#include "ACP.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/**
 * Fonction de démonstration
 */
void demoRobot()
{
	Robot robot;
	while (true) {
		cout << "Ne bouge pas" << endl;
		robot.sendOrder(0, 0);
		getchar();
		cout << "Roues gauche" << endl;
		robot.sendOrder(0.1, 0);
		getchar();
		cout << "Roues droites" << endl;
		robot.sendOrder(0, 0.1);
		getchar();
		cout << "Roues gauche à contre-sens des droites" << endl;
		robot.sendOrder(-0.1, 0.1);
		getchar();
		cout << "Roues gauche et droite à même allure" << endl;
		robot.sendOrder(0.1, 0.1);
		getchar();
	}
}

Mat makeMedianKernel(unsigned size);
void demoCV()
{
	VideoCapture cap(1);

	if (!cap.isOpened()) {
		cout << "Unable to open the device" << endl;
		return;
	}

	unsigned width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	unsigned height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Width : " << width << " Height : " << height << endl;
	Mat frame;
	cap >> frame;
	cout << frame.at<Vec3b>(1, 1) << endl;
	imwrite("/var/www/html/images/test.jpg", frame);

	Mat medianFilteredFrame(height, width, frame.type());
	//Mat medianKernel = (Mat_<float>(3, 3) << 1./9.,1./9.,1./9., 1./9.,1./9.,1./9., 1./9.,1./9.,1./9.);
	Mat medianKernel = makeMedianKernel(3);
	filter2D(frame, medianFilteredFrame, frame.depth(), medianKernel);
	imwrite("/var/www/html/images/test_median.jpg", medianFilteredFrame);
	const Vec3b white_pixel = Vec3b(255, 255, 255);
	/*Mat redFrame(height, width, frame.type());
	Mat greenFrame(height, width, frame.type());
	Mat blueFrame(height, width, frame.type());*/
	Mat distanceFrame(height, width, frame.type());
	Vec3b findedColor(136, 79, 24); // blue, green, red
	double epsilon = 50.0;
	for (unsigned y = 0; y < width; ++y)
	{
		for (unsigned x = 0; x < height; ++x)
		{
			const Vec3b pixel = medianFilteredFrame.at<Vec3b>(x, y);
			/*redFrame.at<Vec3b>(x, y)[0] = pixel[2];
			greenFrame.at<Vec3b>(x, y)[1] = pixel[1];
			blueFrame.at<Vec3b>(x, y)[2] = pixel[0];*/
			if (sqrt(
				pow(pixel[0] - findedColor[0], 2.0) +
				pow(pixel[1] - findedColor[1], 2.0) +
				pow(pixel[2] - findedColor[2], 2.0)) <= epsilon)
			{
				distanceFrame.at<Vec3b>(x, y) = white_pixel;
			}
		}
	}
	/*imwrite("/var/www/html/images/test_R.jpg", redFrame);
	imwrite("/var/www/html/images/test_G.jpg", greenFrame);
	imwrite("/var/www/html/images/test_B.jpg", blueFrame);*/
	imwrite("/var/www/html/images/test_distance.jpg", distanceFrame);
	Mat convolutedFrame(height, width, frame.type());
	Mat kernel = (Mat_<char>(3, 3) << -1, -2, -1,
									   0,  0,  0,
								 	   1,  2,  1);
	filter2D(distanceFrame, convolutedFrame, distanceFrame.depth(), kernel);
	imwrite("/var/www/html/images/test_kernel.jpg", convolutedFrame);
	/*Mat thresholdFrame;
	//cvtColor(frame, testFrame, COLOR_BGR2GRAY);
	threshold(frame, thresholdFrame, 127, 100, 0);
	imwrite("/var/www/html/images/test_threshold.jpg", thresholdFrame);*/
}

Mat makeMedianKernel(unsigned size)
{
	Mat_<float> medianKernel(size, size);
	medianKernel.setTo(1./((float)size * (float)size));
	return medianKernel;
}

int main(int argc, char *argv[])
{
	// demoRobot();
	//demoCV();
	ACP test_acp;
	test_acp.test();
}
