#include <Finger.hpp>
#include <opencv2/core.hpp>
#include <Helpers.hpp>
#include <string>

uint64_t Finger::sMaxID = 0;

Finger::Finger(const cv::RotatedRect& ellipse)
    : mID(sMaxID++) {
    mPreviousPoints.push_back(ellipse.center);
    mCurrentEllipse = ellipse;
}

void Finger::addNewPosition(cv::RotatedRect ellipse) {
    mPreviousPoints.push_back(ellipse.center);
    mCurrentEllipse = ellipse;
    mMark = 0;
}

double Finger::getDistanceTo(const cv::Point2f& point) const {
    auto& lastPoint = mPreviousPoints.back();
    auto vec = point - lastPoint;
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

void Finger::draw(cv::Mat& destination) const {

    if ( mPreviousPoints.size() > 1 )
        for ( size_t index = 0; index < mPreviousPoints.size() - 1; index++ ) {
            auto [a, b] = std::pair{ mPreviousPoints[index], mPreviousPoints[index + 1] };
            cv::line(destination, a, b, cv::Scalar(200, 200, 0));
        }

    cv::Point location = mPreviousPoints.back() + cv::Point2f(2, 2);

    cv::drawMarker(destination, mPreviousPoints.back(), cv::Scalar(255, 0, 0), cv::MARKER_CROSS);
    cv::ellipse(destination, mCurrentEllipse, cv::Scalar(0, 255, 0));
    cv::putText(destination,
        std::to_string(mID),
        mPreviousPoints.back() + cv::Point2f(3, 13),
        cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1
    );

}

const cv::Point2f& Finger::lastPosition() const {
    return mPreviousPoints.back();
}

const uint64_t Finger::id() const {
    return mID;
}

const uint8_t Finger::getAge() const {
    return mMark;
}

void Finger::resetAge() {
    mMark = 0;
}

void Finger::incrementAge() {
    ++mMark;
}

bool Finger::operator==(const Finger& other) const {
    return mID == other.mID;
}

bool Finger::operator!=(const Finger& other) const {
    return mID != other.mID;
}
