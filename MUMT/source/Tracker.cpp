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

Tracker::Tracker(Mat backgroundImage)
    : mFingers(cv::Rect(cv::Point2d{ 0,0 }, backgroundImage.size())) {
    cv::cvtColor(backgroundImage, mBackgroundImage, cv::COLOR_BGRA2GRAY);
    DEBUGFNEX("Tracker initializaed...");
}

void Tracker::trackFingers(const cv::Mat& frame) {

    //clean up fingers that are older than 5 frames
    for ( auto maybeFingerRef : mFingers ) {
        DEBUGFNEX("Finger? %i", maybeFingerRef.has_value());
        if ( !maybeFingerRef.has_value() ) continue;
        auto& finger = maybeFingerRef->get();

        if ( !finger ) continue;

        DEBUGFNEX("id[%lu] coord[%f,%f] age[%i] ",
            finger->id(), finger->lastPosition().x, finger->lastPosition().y, finger->getAge());
        finger->incrementAge();
        if ( finger->getAge() > ceMaxAge ) mFingers.remove(finger);
    }

    Mat maskedImg = frame - mBackgroundImage;
    Mat tempImg1, tempImg2, processedImg;

    blur(maskedImg, tempImg2, Size2i(20, 20));
    tempImg1 = maskedImg - tempImg2;
    blur(tempImg1, tempImg2, Size2i(5, 5));
    threshold(tempImg2, tempImg1, 6, 255, cv::THRESH_BINARY);
    blur(tempImg1, processedImg, Size2i(5, 5));

    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    std::vector<RotatedRect> ellipses;

    findContours(processedImg, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    if ( hierarchy.size() <= 0 ) return;
    for ( int idx = 0; idx >= 0; idx = hierarchy[idx][0] ) {
        if ( contourArea(cv::Mat(contours[idx])) > 30 && contours[idx].size() > 4
        ) {
            auto ellipse = cv::fitEllipse(Mat(contours[idx]));

            ellipses.push_back(ellipse);
        }
    }

    if ( ellipses.empty() ) return;

    DEBUGFNEX("Found %lu Ellipses. Trying to fit to Fingers", ellipses.size());

    for ( auto& ellipse : ellipses ) {
        if ( mFingers.empty() ) {
            DEBUGFNEX("First Ellipse. Insert directly.\n");
            mFingers.insert(std::make_unique<Finger>(ellipse));
            break;
        }

        DEBUGFNEX("Finding Candidates for Ellipse at [%f , %f]", ellipse.center.x, ellipse.center.y);
        auto candidates = mFingers.intersect(ellipse.center, ceMaxFingerDist);

        if ( candidates.size() <= 0 ) {
            DEBUGFNEX("no Candidates found. Insert directly.\n");
            mFingers.insert(std::make_unique<Finger>(ellipse));
        } else {
            DEBUGFNEX("Candidates found. Check for match");
            bool found = false;
            for ( auto& candidate : candidates ) {
                if ( !candidate.get() ) continue;
                DEBUGFNEX("Finger Found id[%lu] age[%i] pos[%f , %f]",
                    candidate.get()->id(),
                    candidate.get()->getAge(),
                    candidate.get()->lastPosition().x, candidate.get()->lastPosition().y
                );

                if ( candidate.get()->getAge() == 0 ) continue;

                mFingers.update(candidate, ellipse);
                found = true;
                break;
            }
            if ( !found ) {
                DEBUGFNEX("Finger is new. Insert directly\n");
                mFingers.insert(std::make_unique<Finger>(ellipse));
            } else {
                DEBUGFNEX("Finger found and updated.\n");
            }
        }

    }

}

void Tracker::update(cv::Mat& frame) {
    //    DEBUGFN(1);
    Mat grayscale;
    cv::cvtColor(frame, grayscale, cv::COLOR_BGR2GRAY);
    trackFingers(grayscale);
}

Mat Tracker::drawInfo() const {
    //    DEBUGFN(1);
    Mat cpimage(mBackgroundImage.size(), mBackgroundImage.type(), cv::Scalar(0, 0, 0));
    Mat image;
    cv::cvtColor(cpimage, image, cv::COLOR_GRAY2BGR);

    for ( const auto& maybeFingerRef : mFingers ) {
        if ( maybeFingerRef.has_value() && maybeFingerRef->get() )
            maybeFingerRef->get()->draw(image);
    }

    return image;
}
