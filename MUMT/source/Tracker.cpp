#include <Tracker.hpp>
#include <Helpers.hpp>

#include <algorithm>
#include <numeric>

using cv::Mat;
using cv::Point;
using cv::RotatedRect;
using cv::Size2i;
using cv::Vec4i;

using cv::blur;
using cv::threshold;
using cv::fitEllipse;
using cv::ellipse;
using cv::findContours;
using cv::contourArea;

#include <iostream>

Tracker::Tracker(Mat backgroundImage) {
    cv::cvtColor(backgroundImage, mBackgroundImage, cv::COLOR_BGRA2GRAY);
}

void Tracker::trackFingers(const cv::Mat& frame) {


    Mat maskedImg = frame - mBackgroundImage;
    Mat tempImg1, tempImg2, processedImg;

    blur(maskedImg, tempImg2, Size2i(20,20));
    tempImg1 = maskedImg - tempImg2;
    blur(tempImg1, tempImg2, Size2i(5,5));
    threshold(tempImg2, tempImg1, 6, 255, cv::THRESH_BINARY);
    blur(tempImg1, processedImg, Size2i(5,5));

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    std::vector<RotatedRect> ellipses;

    findContours(processedImg, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    if(hierarchy.size() <= 0) return;
    for ( int idx = 0; idx >= 0; idx = hierarchy[idx][0] ) {
        if ( contourArea(cv::Mat(contours[idx])) > 30 && contours[idx].size() > 4
        ) {
            auto ellipse = cv::fitEllipse(Mat(contours[idx]));

            ellipses.push_back(ellipse);
        }
    }

    if(ellipses.empty()) return;

    // create array ov indices. everything in here will be marked for deletion
    std::vector<size_t> marks(mFingers.size());
    std::iota(marks.begin(), marks.end(), 0);

    for(auto& ellipse : ellipses) {
        auto point = ellipse.center;
        if(mFingers.empty()){
            mFingers.push_back({point});
            break;
        }

        auto [index, distance] = nearestFinger(point, mFingers);

        if(distance > ceMaxFingerDist) {
            mFingers.push_back({point});
        } else {
            bool markFound = false;
            
            for(size_t markIndex = 0; markIndex < marks.size(); ++markIndex) {
                if(marks[markIndex] == index) {
                    marks.erase(marks.begin() + markIndex);
                    markFound = true;
                    break;
                }
            }

            if(markFound)
                mFingers[index].addNewPosition(point, ellipse);
            else
                mFingers.push_back({point});
        }
    }

    if(marks.empty()) return;

    std::sort(marks.begin(), marks.end(), std::greater<size_t>());
    for(auto index : marks) {
        mFingers.erase(mFingers.begin() + index);
    }


}

void Tracker::update(cv::Mat& frame) {
    Mat grayscale;
    cv::cvtColor(frame, grayscale, cv::COLOR_BGR2GRAY);
    trackFingers(grayscale);
}

Mat Tracker::drawInfo() const {
    // hopefully create Grayscale Mat
    Mat cpimage(mBackgroundImage.size(), mBackgroundImage.type(), cv::Scalar(0,0,0));
    Mat image;
    cv::cvtColor(cpimage, image, cv::COLOR_GRAY2BGR);
    
    for(auto& finger : mFingers) {
        finger.draw(image);
    }

    return image;
}
