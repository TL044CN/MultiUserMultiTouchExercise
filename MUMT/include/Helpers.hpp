#ifndef TT_HELPERS_HPP
#define TT_HELPERS_HPP
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <Finger.hpp>
#include <algorithm>
#include <ranges>

/**
 * @brief Finds the Index of the Finger that is closest to the given Point
 * 
 * @param point the Point to check for
 * @param fingers the vector of Fingers
 * @return tuple<size_t,doublle> the index into the vector of Fingers and its distance
 */
std::tuple<size_t, double> nearestFinger(
    const cv::Point& point,
    const std::vector<Finger>& fingers
);

bool intersectRectCircle(const cv::Rect& rect, const cv::Point& center, double radius);

#endif