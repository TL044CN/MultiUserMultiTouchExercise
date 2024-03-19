#include <Helpers.hpp>
#include <numeric>

std::tuple<size_t, double> nearestFinger(
    const cv::Point& point,
    const std::vector<Finger>& fingers)
{
    size_t closestFinger;
    double closestDistance = std::numeric_limits<double>::max();

    for(size_t index = 0; index < fingers.size(); index++) {
        auto distance = fingers[index].getDistanceTo(point);
        if(distance < closestDistance) {
            closestDistance = distance;
            closestFinger = index;
        }
    }

    return {closestFinger, closestDistance};
}

bool intersectRectCircle(const cv::Rect& rect, const cv::Point& center, double radius) {
    cv::Point closestPoint;
    closestPoint.x = std::max(rect.x, std::min(center.x, rect.x + rect.width));
    closestPoint.y = std::max(rect.y, std::min(center.y, rect.y + rect.height));

    double distance = std::sqrt(std::pow(center.x - closestPoint.x, 2) + std::pow(center.y - closestPoint.y, 2));

//    DEBUGFNEX(1, "rect: [%i,%i,%i,%i] circle: [%i,%i]-%f intersects? %d", rect.x, rect.y, rect.width, rect.height, center.x, center.y, radius, (distance <= radius));
    return distance <= radius;
}

bool inCircle(const cv::Point& point, const cv::Point& center, double radius) {
    return radius >= cv::norm(point - center);
}
