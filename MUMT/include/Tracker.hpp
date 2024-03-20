#ifndef TT_TRACKER_HPP
#define TT_TRACKER_HPP

#include "opencv2/opencv.hpp"
#include "Finger.hpp"
#include "QuadTree.hpp"

#include <vector>

constexpr double ceMaxFingerDist = 35;
constexpr uint64_t ceMaxAge = 1;

/**
 * @brief Core of the Tracking system
 *
 */
class Tracker {
private:
    cv::Mat mBackgroundImage;
    QuadTree mFingers;
//    std::vector<Finger> mFingers;

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

    /**
     * @brief Returns the Finger Storage
     * 
     * @return const QuadTree& const ref to the finger storage
     */
    const QuadTree& getFingers() const;

};

#endif