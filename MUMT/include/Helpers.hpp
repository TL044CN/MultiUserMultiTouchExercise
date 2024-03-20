#ifndef TT_HELPERS_HPP
#define TT_HELPERS_HPP
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Finger.hpp"
#include <algorithm>
#include <ranges>

#include <iostream>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#if DEBUG
#define DEBUGFN(i)        std::cout << __FILENAME__ << " : " << __func__ << "[" << std::to_string(i) <<"]\n"
#define DEBUGFNEX(format ...)\
    std::cout << __FILENAME__ << " : " << __func__ << '\t'; \
    std::printf(format);\
    std::cout << std::endl
#else
#define DEBUGFN(i) ;
#define DEBUGFNEX(format,...);
#endif



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

/**
 * @brief Checks if a Circle intersects with a Circle
 *
 * @param rect the Rect to intersect with the circle
 * @param center the center of the circle
 * @param radius the radius of the circle
 * @return true Intersection occured
 * @return false no Intersection occured
 */
bool intersectRectCircle(const cv::Rect& rect, const cv::Point& center, double radius);

/**
 * @brief checks if a point lies inside a circle
 *
 * @param point the point to check
 * @param center the center of the circle
 * @param radius the radius of the circle
 * @return true the point lies within the circle
 * @return false the point doesnt lie inside the circle
 */
bool inCircle(const cv::Point& point, const cv::Point& center, double radius);

#endif