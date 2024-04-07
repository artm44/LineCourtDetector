#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "CourtDetector.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	string path;

	if (argc > 1)
	{
		path = argv[1];
	}
	else
	{
		cout << "Input path to image" << endl;
		cin >> path;
	}

	Mat image = imread(path);

	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get();
		return -1;
	}

	CourtDetector courtDetector(image);

	courtDetector.detect();

	std::vector<cv::Point> corners = courtDetector.getCorners();

	if (corners.size() != 0) {

		std::vector<std::string> namesCorners = { "Top Left", "Top Right", "Bottom Left", "Bottom Right" };

		for (size_t i = 0; i < 4; i++)
		{
			std::cout << namesCorners[i] << ": " << corners[i] << endl;
		}

		cv::resize(image, image, cv::Size(1080, 720));

		for (const auto& corner : corners) {
			cv::circle(image, cv::Point(corner.x, corner.y), 5, cv::Scalar(0, 255, 0), -1);
		}
	}
	else {
		std::cout << "Corners Not Found!" << endl;
	}

	String windowName = "Tennis Court";
	namedWindow(windowName);
	imshow(windowName, image);
	waitKey(0);
	destroyWindow(windowName);

	return 0;
}