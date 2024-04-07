#pragma once
#include <opencv2/opencv.hpp>

// Клас для обработки линий
class LineHandler
{
	std::vector<std::vector<cv::Vec4i>> classifyLines(const std::vector<cv::Vec4i>& lines);

	void mergeLines(std::vector<std::vector<cv::Vec4i>>& linesClassification);
public:
	std::vector<std::vector<cv::Vec4i>> findLines(cv::Mat& image);
};