#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class CourtDetector
{
	/*const int WIDTH = 1080;
	const int HEIGHT = 720;*/

	cv::Mat original_image;
	cv::Mat image;

	std::vector<cv::Point> corners;

	cv::Mat preprocessImage(cv::Mat& img);

	std::vector<std::vector<cv::Point>> getContours(cv::Mat& gray, int threshold);

	void findCourtLines(const std::vector<std::vector<cv::Vec4i>>& linesClassification);

public:
	CourtDetector(cv::Mat img);

	void detect();

	std::vector<cv::Point> getCorners() { return corners; }
};
