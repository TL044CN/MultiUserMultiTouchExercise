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

    std::vector<cv::Point2f> mPreviousPoints;
    cv::RotatedRect mCurrentEllipse;

    uint8_t mMark = 0;

public:
    /**
     * @brief Construct a new Finger object
     *
     * @param ellipse the first ellipse that was detected
     */
    Finger(const cv::RotatedRect& ellipse);

public:
    /**
     * @brief add a new Position to the Finger
     *
     * @param ellipse the Ellipse of the position
     */
    void addNewPosition(cv::RotatedRect ellipse);

    /**
     * @brief get the distance from the last point to the given point
     *
     * @param point the Point to calculate the distance to
     * @return double the distance to the point
     */
    double getDistanceTo(const cv::Point2f& point) const;

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
    const cv::Point2f& lastPosition() const;

    /**
     * @brief returns the ID of the Finger
     * 
     * @return const uint64_t the ID of the Finger
     */
    const uint64_t id() const;

    /**
     * @brief Get the Mark value
     * 
     * @return const uint8_t the mark value
     */
    const uint8_t getAge() const;

    /**
     * @brief Resets the Mark of the Finger
     */
    void resetAge();

    /**
     * @brief Increments the Age of a Finger
     */
    void incrementAge();

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