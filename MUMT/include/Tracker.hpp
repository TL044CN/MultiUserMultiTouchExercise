#ifndef TT_TRACKER_HPP
#define TT_TRACKER_HPP

#include <opencv2/opencv.hpp>
#include <Finger.hpp>
#include <vector>

constexpr double ceMaxFingerDist = 29;

/**
 * @brief Core of the Tracking system
 *
 */
class Tracker {
private:
    cv::Mat mBackgroundImage;
    std::vector<Finger> mFingers;

public:
    /**
     * @brief Construct a new Tracker
     *
     */
    Tracker(cv::Mat backgroundImage);

private:
    /**
     * @brief Runs the Tracking Algorithm
     *
     * @param frame the Frame to analyze and use for tracking
     */
    void trackFingers(const cv::Mat& frame);

public:
    /**
     * @brief Update the Tracker with a new Frame
     *
     * @param frame
     */
    void update(cv::Mat& frame);

    /**
     * @brief Creates an Image, sized like the background image that includes
     *        all the Information about the tracked Fingers
     * 
     * @return cv::Mat 
     */
    cv::Mat drawInfo() const;

};

#endif