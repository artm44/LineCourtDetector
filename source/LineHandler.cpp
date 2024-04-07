#include "LineHandler.h"
#include "Constants.h"

std::vector<std::vector<cv::Vec4i>> LineHandler::classifyLines(const std::vector<cv::Vec4i>& lines)
{
    std::vector<std::vector<cv::Vec4i>> linesClassification(2); // Два типа: горизонтальные и вертикальные линии

    for (const auto& line : lines) {
        int dx = std::abs(line[0] - line[2]);
        int dy = std::abs(line[1] - line[3]);
        if (dx > dy) {
            linesClassification[0].push_back(line);
        }
        else {
            linesClassification[1].push_back(line);
        }
    }

    return linesClassification;
}

void LineHandler::mergeLines(std::vector<std::vector<cv::Vec4i>>& linesClassification)
{
    int d = 15;

    // Для горизонтальных линий
    for (size_t i = 0; i < linesClassification[0].size(); ++i) {
        if (linesClassification[0][i][0] > linesClassification[0][i][2]) {
            std::swap(linesClassification[0][i][0], linesClassification[0][i][2]);
            std::swap(linesClassification[0][i][1], linesClassification[0][i][3]);
        }
    }

    std::sort(linesClassification[0].begin(), linesClassification[0].end(), [](const cv::Vec4i& a, const cv::Vec4i& b) {
        return a[0] < b[0];
        });

    size_t i = 0;
    while (i < linesClassification[0].size()) {
        size_t j = i + 1;
        while (j < linesClassification[0].size()) {
            if (std::abs(linesClassification[0][i][3] - linesClassification[0][j][1]) < d) {
                linesClassification[0][i][1] = (linesClassification[0][i][1] + linesClassification[0][j][1]) / 2;
                linesClassification[0][i][3] = (linesClassification[0][i][3] + linesClassification[0][j][3]) / 2;
                linesClassification[0][i][2] = linesClassification[0][j][2];
                linesClassification[0].erase(linesClassification[0].begin() + j);
            }
            else {
                ++j;
            }
        }
        ++i;
    }

    // Для вертикальных линий
	for (size_t i = 0; i < linesClassification[1].size(); i++)
	{
		if (linesClassification[1][i][1] < linesClassification[1][i][3])
		{
			cv::Point buf = cv::Point(linesClassification[1][i][0], linesClassification[1][i][1]);
			linesClassification[1][i][0] = linesClassification[1][i][2];
			linesClassification[1][i][1] = linesClassification[1][i][3];
			linesClassification[1][i][2] = buf.x;
			linesClassification[1][i][3] = buf.y;
		}
	}
	for (int i = 0; i < linesClassification[1].size() - 1; i++) {
		for (int j = 0; j < linesClassification[1].size() - i - 1; j++) {
			if (linesClassification[1][j][1] < linesClassification[1][j + 1][1]) {
				cv::Vec4i temp = linesClassification[1][j];
				linesClassification[1][j] = linesClassification[1][j + 1];
				linesClassification[1][j + 1] = temp;
			}
		}
	}
	for (size_t i = 0; i < linesClassification[1].size(); i++)
	{
		for (size_t j = i + 1; j < linesClassification[1].size(); j++)
		{
			int x1, x2;
			x1 = ((HEIGHT / 2.0 - linesClassification[1][i][1]) / (linesClassification[1][i][3] - linesClassification[1][i][1])) * (linesClassification[1][i][2] - linesClassification[1][i][0]) + linesClassification[1][i][0];
			x2 = ((HEIGHT / 2.0 - linesClassification[1][j][1]) / (linesClassification[1][j][3] - linesClassification[1][j][1])) * (linesClassification[1][j][2] - linesClassification[1][j][0]) + linesClassification[1][j][0];
			if (abs(x1 - x2) < d)
			{
				if (linesClassification[1][i][3] > linesClassification[1][j][3]) {
					linesClassification[1][i] = cv::Vec4i(linesClassification[1][i][0], linesClassification[1][i][1],
						linesClassification[1][j][2], linesClassification[1][j][3]);
				}
				linesClassification[1].erase(linesClassification[1].begin() + j);
				j--;
			}
		}
	}
}

std::vector<std::vector<cv::Vec4i>> LineHandler::findLines(cv::Mat& image)
{
	// Выполнение преобразования Хафа для поиска линий
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(image, lines, 1, CV_PI / 180, 100, 50, 10);

    std::vector<std::vector<cv::Vec4i>> linesClassification = classifyLines(lines);

    mergeLines(linesClassification);

	return linesClassification;
}
