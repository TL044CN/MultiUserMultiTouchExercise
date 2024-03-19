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
