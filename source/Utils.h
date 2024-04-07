#pragma once
#include <opencv2/opencv.hpp>
#include "Constants.h"

// ������� ��� ���������� ���� ������� �����
double angleOfLine(const cv::Vec4i& line) {
    double dx = line[2] - line[0];
    double dy = line[3] - line[1];
    return atan2(dx, dy);
}

// ������� ��� ���������� ����������� ���� �����
cv::Point lineIntersection(const cv::Vec4i& line1, const cv::Vec4i& line2) {
    double x1 = line1[0], y1 = line1[1], x2 = line1[2], y2 = line1[3];
    double x3 = line2[0], y3 = line2[1], x4 = line2[2], y4 = line2[3];

    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (denom == 0) // ����� ����������� ��� ���������
        return cv::Point(-1, -1);

    double px = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denom;
    double py = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denom;

    return cv::Point(px, py);
}

// ������� ��� ���������� ������� �������� �� ������� �����
double computeTrapezoidArea(const cv::Point& top_left, const cv::Point& top_right,
    const cv::Point& bottom_left, const cv::Point& bottom_right) {
    // ���������� ������ ��������
    double height = std::max(bottom_left.y - top_left.y, bottom_right.y - top_right.y);

    // ���������� ��������� ��������
    double base1 = cv::norm(top_left - top_right);
    double base2 = cv::norm(bottom_left - bottom_right);

    // ������� ��������: (����� ��������� * ������) / 2
    return (base1 + base2) * height / 2.0;
}


bool checkIntersections(const std::vector<cv::Point>& intersections) {
    for (const auto& intersection : intersections) {
        if (intersection.x < 0 || intersection.x > WIDTH || intersection.y < 0 || intersection.y > HEIGHT) {
            return false; // ������� ����������� �� ��������� �����������
        }
    }
    return true; // ��� ����������� �� ���������
}

// ������� ��� ���������� ���-�� ������� ��������� �����������
int getCntRealIntersections(cv::Vec4i& h1, cv::Vec4i& h2, cv::Vec4i& v1, cv::Vec4i& v2)
{
    int cnt = 0;
    const int delta = 15;

    int top_y = (h1[1] + h1[3]) / 2, bottom_y = (h2[1] + h2[3]) / 2;

    if (std::abs(top_y - std::min(v1[1], v1[3])) < delta)
        cnt++;
    if (std::abs(top_y - std::min(v2[1], v2[3])) < delta)
        cnt++;
    if (std::abs(bottom_y - std::max(v1[1], v1[3])) < delta)
        cnt++;
    if (std::abs(bottom_y - std::max(v2[1], v2[3])) < delta)
        cnt++;

    return cnt;
}