#include <string>
#include <sstream>
#include "ImageProcessing.h"

using namespace std;
using namespace cv;

ImageProcessing::ImageProcessing()
{}

void ImageProcessing::drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale)
{
	double angle;
	double hypotenuse;
	angle = atan2( (double) p.y - q.y, (double) p.x - q.x ); // angle in radians
	hypotenuse = sqrt( (double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
	//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
	//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
	// Here we lengthen the arrow by a factor of scale
	q.x = (int) (p.x - scale * hypotenuse * cos(angle));
	q.y = (int) (p.y - scale * hypotenuse * sin(angle));
	line(img, p, q, colour, 1, CV_AA);
	// create the arrow hooks
	p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
	p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
	line(img, p, q, colour, 1, CV_AA);
}
double ImageProcessing::getOrientation(const vector<Point> &pts, Mat &img)
{
	//Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	Mat data_pts = Mat(sz, 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//Perform PCA analysis
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
	//Store the center of the object
	Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
	              static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i)
	{
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
		                        pca_analysis.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
	}
	// Draw the principal components
	circle(img, cntr, 3, Scalar(255, 0, 255), 2);
	Point p1 = cntr + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	Point p2 = cntr - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	drawAxis(img, cntr, p1, Scalar(0, 255, 0), 1);
	drawAxis(img, cntr, p2, Scalar(255, 255, 0), 1);
	double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
	return angle;
}
void ImageProcessing::test()
{
	// Load image
	Mat src = imread("/var/www/html/images/test_distance.jpg");
	// Check if image is loaded successfully
	if(!src.data || src.empty())
	{
		cout << "Problem loading image!!!" << endl;
		//return EXIT_FAILURE;
		return;
	}
	//imshow("src", src);
	// Convert image to grayscale
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	// Convert image to binary
	Mat bw;
	threshold(gray, bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	// Find all the contours in the thresholded image
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
	findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	for (size_t i = 0; i < contours.size(); ++i)
	{
		// Calculate the area of each contour
		double area = contourArea(contours[i]);
		// Ignore contours that are too small or too large
		if (area < 1e2 || 1e5 < area) continue;
		// Draw each contour only for visualisation purposes
		drawContours(src, contours, static_cast<int>(i), Scalar(0, 0, 255), 2, 8, hierarchy, 0);
		// Find the orientation of each shape
		getOrientation(contours[i], src);
	}
	//imshow("Output 1", src);
}

Mat ImageProcessing::makeMedianKernel(unsigned size)
{
	Mat_<float> medianKernel(size, size);
	medianKernel.setTo(1./((float)size * (float)size));
	return medianKernel;
}

void ImageProcessing::cleanImage(const Mat &img, Mat &out)
{
	Mat medianKernel = makeMedianKernel(3);
	filter2D(img, out, img.depth(), medianKernel);
}

void ImageProcessing::extractColorAsBlackWhite(
	const Mat &img,
	Mat &out,
	const Vec3b &color,
	double colorThreshold)
{
	const Vec3b whitePixel(255, 255, 255);
	unsigned width = img.cols;
	unsigned height = img.rows;
	Mat gray(height, width, img.type());
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			const Vec3b pixel = img.at<Vec3b>(x, y);
			if (sqrt(
				pow(pixel[0] - color[0], 2.0) +
				pow(pixel[1] - color[1], 2.0) +
				pow(pixel[2] - color[2], 2.0)) <= colorThreshold)
			{
				gray.at<Vec3b>(x, y) = whitePixel;
			}
		}
	}
	Mat trueGray;
	cvtColor(gray, trueGray, COLOR_BGR2GRAY);
	imshow("gray", gray);
	imshow("true gray", trueGray);
	threshold(trueGray, out, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
}

vector<vector<Point> > ImageProcessing::getRegions(Mat &img)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(img, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	return contours;
}

void ImageProcessing::performRegionPCA(const vector<Point> &pts, Axis &axis)
{
	//Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	Mat data_pts = Mat(sz, 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//Perform PCA analysis
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
	//Store the center of the object
	Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
	              static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i)
	{
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
		                        pca_analysis.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
	}
	// Draw the principal components
	//circle(img, cntr, 3, Scalar(255, 0, 255), 2);
	Point p1 = cntr + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	Point p2 = cntr - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	axis.center = cntr;
	axis.p1 = p1;
	axis.p2 = p2;
	/*drawAxis(img, cntr, p1, Scalar(0, 255, 0), 1);
	drawAxis(img, cntr, p2, Scalar(255, 255, 0), 1);
	double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
	return angle;*/
}

Axis ImageProcessing::compute(
	const Mat &img,
	const Vec3b &color,
	double colorThreshold,
	double minArea)
{
	Mat image;
	cleanImage(img, image);
	Mat bw;
	extractColorAsBlackWhite(image, bw, color, colorThreshold);
	imshow("black and white", bw);

	vector<vector<Point> > contours = getRegions(bw);
	vector<Axis> axisVec;
	Axis mean;

	// compute PCA on each sufficient large region
	for (size_t i = 0; i < contours.size(); ++i)
	{
		// Calculate the area of each contour
		double area = contourArea(contours[i]);
		// Ignore contours that are too small or too large
		if (area < minArea) continue;
		Axis axis;
		axis.area = area;
		performRegionPCA(contours[i], axis);
		axisVec.push_back(axis);
	}

	// compute mean of each PCA
	for (size_t i = 0; i < axisVec.size(); ++i)
	{
		// FAIRE UNE MOYENNE PONDEREE
		Axis axis = axisVec[i];

		circle(image, axis.center, 3, Scalar(255, 0, 255), 2);
		drawAxis(image, axis.center, axis.p1, Scalar(0, 255, 0), 0.4);
		drawAxis(image, axis.center, axis.p2, Scalar(255, 255, 0), 0.4);
	}
	if (axisVec.size() >= 1)
		mean = axisVec[0];

	imshow("Output 1", image);
	return mean;
}
