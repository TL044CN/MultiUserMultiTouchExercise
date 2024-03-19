#ifndef TT_FINGER_HPP
#define TT_FINGER_HPP

#include <opencv2/core/types.hpp>
#include <cstdint>
#include <vector>
#include <functional>

/**
 * @brief Represents a Finger in the Tracking System
 *
 */
class Finger {
private:
    static uint64_t sMaxID;
    uint64_t mID;

    std::vector<cv::Point> mPreviousPoints;
    cv::RotatedRect mCurrentEllipse;

public:
    /**
     * @brief Construct a new Finger object
     *
     * @param origin the first Point the Finger was detected at
     */
    Finger(cv::Point origin);

public:
    /**
     * @brief add a new Position to the Finger
     *
     * @param point the Point at the new position
     */
    void addNewPosition(cv::Point point, cv::RotatedRect ellipse);

    /**
     * @brief get the distance from the last point to the given point
     *
     * @param point the Point to calculate the distance to
     * @return double the distance to the point
     */
    double getDistanceTo(cv::Point point) const;

    /**
     * @brief Draw the Path and ID of the Image
     *
     * @param destination the Frame to draw the path in
     * @param grayscale weather or not the frame is grayscale
     */
    void draw(cv::Mat& destination) const;

    /**
     * @brief Returns the newest Position of the Finger
     * 
     * @return const cv::Point& the latest Position
     */
    const cv::Point& lastPosition() const;

    /**
     * @brief returns the ID of the Finger
     * 
     * @return const uint64_t the ID of the Finger
     */
    const uint64_t id() const;

    /**
     * @brief Cheks if 2 fingers have the same ID
     * 
     * @param other the other Finger
     * @return true fingers have the same ID, they should be the same
     * @return false finger dont have the same ID, they should be different
     */
    bool operator==(const Finger& other) const;

    /**
     * @brief Cheks if 2 fingers have different IDs
     * 
     * @param other the other Finger
     * @return true finger dont have the same ID, they should be different
     * @return false fingers have the same ID, they should be the same
     */
    bool operator!=(const Finger& other) const;

};


#endif