#include "CourtDetector.h"
#include "LineHandler.h"
#include "Utils.h"
#include "Constants.h"


cv::Mat CourtDetector::preprocessImage(cv::Mat& img)
{
    // Применение фильтра среднего сдвига
    cv::Mat shifted;
    cv::pyrMeanShiftFiltering(image, shifted, 21, 31);

    // Преобразование изображения в градации серого
    cv::Mat gray;
    cv::cvtColor(shifted, gray, cv::COLOR_BGR2GRAY);

    // Применение гауссового размытия
    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(5, 5), 0);

    return blur;
}

std::vector<std::vector<cv::Point>> CourtDetector::getContours(cv::Mat& gray, int threshold)
{
    // Применение оператора Canny для обнаружения краев
    cv::Mat edged;
    cv::Canny(gray, edged, 30, 100);

    // Поиск контуров
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edged, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    std::vector<std::vector<cv::Point>> filteredContours;

    int threshCountourSize = 200;

    for (const auto& contour : contours) {

        if (contour.size() < threshCountourSize) {
            continue;
        }

        bool flag = true;
        for (const auto& point : contour) {
            if (point.x < threshold || point.x > WIDTH - threshold || point.y < threshold || point.y > HEIGHT - threshold) {
                flag = false;
                break;
            }
        }

        if (flag) {
            filteredContours.push_back(contour);
        }
    }

    return filteredContours;
}

void CourtDetector::findCourtLines(const std::vector<std::vector<cv::Vec4i>>& linesClassification)
{
    double bestArea = 0;

    for (size_t i = 0; i < linesClassification[0].size() - 1; i++)
    {
        for (size_t j = i + 1; j < linesClassification[0].size(); j++)
        {
            for (size_t k = 0; k < linesClassification[1].size() - 1; k++)
            {
                for (size_t l = k + 1; l < linesClassification[1].size(); l++)
                {
                    cv::Vec4i h1 = linesClassification[0][i], h2 = linesClassification[0][j];

                    // Проверка на параллельность
                    if (std::abs(angleOfLine(h1) - angleOfLine(h2)) > 0.07)
                        continue;

                    cv::Vec4i v1 = linesClassification[1][k], v2 = linesClassification[1][l];

                    if (std::abs(angleOfLine(v1) + angleOfLine(v2)) > 0.17)
                        continue;

                    if ((h1[1] + h1[3]) / 2 > (h2[1] + h2[3]) / 2)
                    {
                        std::swap(h1, h2);
                    }

                    if ((v1[0] + v1[2]) / 2 > (v2[0] + v2[2]) / 2)
                    {
                        std::swap(h1, h2);
                    }

                    std::vector<cv::Point> intersections = { lineIntersection(h1, v1), lineIntersection(h1, v2),
                                                 lineIntersection(h2, v1), lineIntersection(h2, v2) };

                    if (!checkIntersections(intersections))
                        continue;

                    if (cntRealIntersections(h1, h2, v1, v2) < 3)
                        continue;

                    double area = computeTrapezoidArea(intersections[0], intersections[1], intersections[2], intersections[3]);

                    if (area > bestArea) {
                        bestArea = area;
                        this->corners = intersections;
                    }
                }
            }
        }
    }
}

CourtDetector::CourtDetector(cv::Mat img) : original_image(img) {
    cv::resize(img, this->image, cv::Size(WIDTH, HEIGHT));
}

void CourtDetector::detect()
{
    cv::Mat preparedImg = preprocessImage(this->image);

    std::vector<std::vector<cv::Point>> contours = getContours(preparedImg, 20);

    // Создание пустого изображения для контуров
    cv::Mat contoursImage = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::drawContours(contoursImage, contours, -1, cv::Scalar(255), 1);

    LineHandler lineHandler;
    std::vector<std::vector<cv::Vec4i>> linesClassification = lineHandler.findLines(contoursImage);

    cv::Mat buf = image;

    // Отрисовка горизонтальных линий красным цветом
    for (const auto& line : linesClassification[0]) {
        cv::line(buf, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 0, 255), 4);
    }

    // Отрисовка вертикальных линий зеленым цветом
    for (const auto& line : linesClassification[1]) {
        cv::line(buf, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 255, 0), 4);
    }

    cv::imshow("windowName", buf); // Show our image inside the created window.


    findCourtLines(linesClassification);
}